#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>
#include <MysqlCpp/Responses/AuthMoreData.hpp>
#include <MysqlCpp/Responses/AuthSwitchRequest.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Responses/Response.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <MysqlCpp/Support/Packet.hpp>

#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <bitset>
#include <string>

using MysqlCpp::Responses::AuthMoreData;
using MysqlCpp::Responses::AuthSwitchRequest;
using MysqlCpp::Responses::Error;
using MysqlCpp::Responses::Ok;
using MysqlCpp::Responses::Response;
using MysqlCpp::Support::BufferParser;
using MysqlCpp::Support::Packet;

typedef unsigned char byte;
#define SALT_LENGTH 20

namespace MysqlCpp
{

Connection::Connection(std::string hostname, unsigned int port, std::string username, std::string password, bool sslEnabled)
{
    this->hostname = hostname;
    this->port = port;
    this->username.reserve(username.length());
    std::for_each(username.begin(), username.end(), [this](char c) { this->username.push_back(std::byte(c)); });
    this->password = password;
    this->sslEnabled = sslEnabled;
}

Connection::~Connection()
{
    SSL_free(this->ssl);
    this->ssl = NULL;
    close(this->socketHandle);
    this->socketHandle = 0;
    SSL_CTX_free(this->sslCtx);
    this->sslCtx = NULL;
}

std::shared_ptr<Response> Connection::connect()
{
    if (this->connected)
    {
        return 0;
    }

    struct sockaddr_in server;
    struct in_addr ipv4addr;

    this->socketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    struct hostent *hp = ::gethostbyname(this->hostname.c_str());

    ::bcopy(hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
    server.sin_port = htons(this->port);

    auto connectResponse = ::connect(this->socketHandle, (const sockaddr *)&server, sizeof(server));

    if (connectResponse < 0)
    {
        return std::make_shared<Error>(connectResponse, "Unable to connect to socket.");
    }

    this->receiveInitialHandshake();

    // Handle SSL
    if (this->sslEnabled)
    {
        this->sendSslRequest();
        this->exchangeSsl();
    }

    bool shouldSendHandshakeResponse = true;
    std::shared_ptr<MysqlCpp::Responses::Response> response;
    do
    {
        if (shouldSendHandshakeResponse)
        {
            this->sendHandshakeResponse();
        }
        shouldSendHandshakeResponse = true;

        // Read packets
        this->parser = std::make_shared<BufferParser>(this->readPackets());
        response = Response::build(*this->parser, this);

        auto errResponse = std::dynamic_pointer_cast<MysqlCpp::Responses::Error>(response);
        auto okResponse = std::dynamic_pointer_cast<MysqlCpp::Responses::Ok>(response);
        auto authSwitchRequestResponse = std::dynamic_pointer_cast<MysqlCpp::Responses::AuthSwitchRequest>(response);
        auto authMoreData = std::dynamic_pointer_cast<MysqlCpp::Responses::AuthMoreData>(response);

        if (errResponse)
        {
            std::cout << "Error: " << errResponse->getErrorMessage() << std::endl;

            return errResponse;
        }

        if (authSwitchRequestResponse)
        {
            std::cout << "Name: " << authSwitchRequestResponse->getPluginName() << std::endl
                      << "Data: " << authSwitchRequestResponse->getPluginData() << std::endl;
            return authSwitchRequestResponse;
        }

        if (authMoreData)
        {
            std::cout << "Auth more data" << std::endl;
            auto data = authMoreData->getData();

            switch (data[0])
            {
            case 3:
            {
                std::cout << "Got \"\\x03\", which I think means \"yeah we gotchu\"" << std::endl;
                shouldSendHandshakeResponse = false;
                break;
            }
            case 4:
            {
                std::cout << "Got \"End of Transmission\" signal from server. This probably means we need to do full authentication." << std::endl
                          << std::endl;

                // Request server public key. Docs say to use 0x01, however 0x02 is what actually needs to be sent (discovered via packet sniffing)
                // https://dev.mysql.com/doc/internals/en/public-key-retrieval.html
                Packet packet;
                packet.write(0x2, 1);
                this->write(packet);

                // Read the server's public key.
                // TODO: Have a way to offer this for end-users to cache it. Maybe std::functional could do it
                // TODO: Allow the public key in the constructor, so we can skip asking for it at all
                this->parser = std::make_shared<BufferParser>(this->readPackets());
                authMoreData = std::dynamic_pointer_cast<MysqlCpp::Responses::AuthMoreData>(Response::build(*this->parser, this));

                // Extract the public key from the response
                std::string publicKey = authMoreData->getData();

                // Create a new packet and send the RSA encrypted password back to the server
                std::vector<std::byte> textToEncrypt;
                for (unsigned int i = 0; i <= this->password.length(); i++) // We use <= to XOR in the trailing NULL character
                {
                    // XOR the scramble and the password to start the hashing
                    // Modulus division when accessing this->authPluginData to ensure we don't access beyond the length of the plugin data
                    textToEncrypt.push_back(std::byte(this->password[i] ^ this->authPluginData[i % this->authPluginData.length()]));
                }
                Packet rsaResponsePacket;
                std::vector<std::byte> encrypted = this->rsaEncryptPassword(textToEncrypt, publicKey);
                rsaResponsePacket.write(encrypted);
                this->write(rsaResponsePacket);

                this->parser = std::make_shared<BufferParser>(this->readPackets());
                this->sequenceId = 0;
                return Response::build(*this->parser, this);
            }
            default:
            {
                std::cout << "Unknown signal received in AuthMoreData response:" << std::endl
                          << std::endl;

                for (unsigned int i = 0; i < data.size(); i++)
                {
                    std::cout << std::bitset<8>(data[i]) << std::endl;
                }
            }
            }
        }
    } while (!::instanceOf<MysqlCpp::Responses::Ok>(response));

    this->connected = true;
    this->phase = Connection::Phases::COMMAND;
    this->sequenceId = 0;

    return response;
}

std::shared_ptr<MysqlCpp::Responses::Response> Connection::disconnect()
{
    Packet packet;
    packet.write(Connection::ServerCommands::QUIT, 1);
    this->write(packet);

    this->parser = std::make_shared<BufferParser>(this->readPackets());
    return Response::build(*this->parser, this);
}

void Connection::receiveInitialHandshake()
{
    // Read the packets from the server and build parser
    this->parser = std::make_shared<Support::BufferParser>(this->readPackets());

    // Extract protocol version
    int protocolVersion = this->parser->uInt<1>();

    if (protocolVersion != 10)
    {
        this->parser->seek(0);
        std::shared_ptr<Response> error = std::make_shared<Error>(*this->parser, this);
        std::cout << error << std::endl;
        throw std::string("Protocol version not compatible. (").append(std::to_string(protocolVersion)).append(")");
    }

    // Extract human readable status
    auto humanReadableStatus = this->parser->string<0>();

    // Extract thread ID
    unsigned int threadId = this->parser->uInt<4>();

    // Extract first 8 bytes (null terminated) of authPluginData, also known as the scramble
    this->authPluginData = this->parser->string<0>();

    auto capabilityFlagsLower = this->parser->uInt<2>();

    // Extract server character set
    unsigned int characterSet = this->parser->uInt<1>();

    // Extract server status flags
    this->status = this->parser->uInt<2>();

    this->serverCapabilities = (this->parser->uInt<2>() << 16) | capabilityFlagsLower;

    int authPluginDataLength = 0;
    if (this->capable(Connection::Capabilities::CLIENT_PLUGIN_AUTH))
    {
        authPluginDataLength = this->parser->uInt<1>() - 8;
    }
    else
    {
        this->parser->uInt<1>();
    }

    // 10 bytes are always blank
    this->parser->string<10>();

    // If there is additional length to the authPluginData, pull it in
    unsigned int max = std::max(13, authPluginDataLength);
    unsigned int i = this->parser->getCursor();
    this->authPluginData += this->parser->string(i, max);
    this->authPluginData.pop_back(); // Remove the trailing \x00. This may cause problems if it's not ALWAYS a trailing \x00, and instead is partof the salt
    this->parser->seek(i);

    if (this->capable(Connection::Capabilities::CLIENT_PLUGIN_AUTH))
    {
        this->serverRequestAuthMethod = this->parser->string<0>();
    }
}

void Connection::sendSslRequest()
{
    Packet packet;
    if (this->capable(Capabilities::CLIENT_PROTOCOL_41))
    {
        packet.write(this->clientCapabilities & this->serverCapabilities, 4); // Capabilities flags
        packet.write(0xFFFFFFFF, 4);                                          // Max packet size (1's all the way through)
        packet.write(this->characterSet, 1);                                  // Character set
        packet.write(std::to_vector(std::string(23, 0)));                     // 23 character filler (0's all the way through)
    }
    else
    {
        packet.write(this->clientCapabilities & this->serverCapabilities, 2); // Lower 2 capabilities
        packet.write(0xFFFFFF, 3);                                            // Max packet size (1's all the way through)
    }

    this->write(packet, false);
}

void Connection::exchangeSsl()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    this->sslCtx = SSL_CTX_new(TLSv1_2_client_method());

    if (this->sslCtx == nullptr)
    {
        ERR_print_errors_fp(stderr);
        return;
    }

    this->ssl = SSL_new(this->sslCtx);

    if (this->ssl == nullptr)
    {
        std::cout << "SSL handle is NULL" << std::endl;
    }

    SSL_set_fd(this->ssl, this->socketHandle);
    SSL_connect(this->ssl);

    X509 *cert = SSL_get_peer_certificate(ssl);

    if (cert != NULL)
    {
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        delete line;
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        delete line;
        X509_free(cert);
    }
    else
    {
        std::cout << "Info: No client certificates configured." << std::endl;
    }
}

void Connection::sendHandshakeResponse()
{
    std::cout << "Sending handshake response" << std::endl;
    Packet packet;
    packet.write(this->clientCapabilities & this->serverCapabilities, 4); // Capabilities
    packet.write(0xFFFFFFFF, 4);                                          // Max packet size
    packet.write(this->characterSet, 1);
    packet.write(std::to_vector(std::string(23, 0))); // Filler, all 0's
    packet.write(this->username, true); // Null terminate username

    if (this->clientAuthMethod == "caching_sha2_password" && this->password.length() > 0 && this->authResponse.size() == 0 && this->supportsAuthMethod(this->serverRequestAuthMethod))
    {
        this->authResponse = this->generateCachingSha2AuthResponse(this->password, this->authPluginData);
    }

    // Auth response, usually the caching_sha2_password response
    if (this->capable(Connection::Capabilities::CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA))
    {
        packet.writeLengthEncoded(this->authResponse); // Write length encoded auth response
    }
    else
    {
        packet.write(this->authResponse.size(), 1); // Write length of auth response as int<1>
        packet.write(this->authResponse); // Write auth response
    }

    if (this->capable(Connection::Capabilities::CLIENT_CONNECT_WITH_DB))
    {
        packet.write(std::to_vector(this->database), true);
    }

    if (this->capable(Connection::Capabilities::CLIENT_PLUGIN_AUTH))
    {
        packet.write(std::to_vector(this->clientAuthMethod), true);
    }

    if (this->capable(Connection::Capabilities::CLIENT_CONNECT_ATTRS))
    {
        packet.writeLengthEncoded(0);
        // TODO
        // Length encode each key and value of the attributes to send to server as string<lenenc>
        // Length encode the total length of all length encoded keys and attributes as int<lenenc>
    }

    this->write(packet, false);
}

std::shared_ptr<Response> Connection::query(const std::string &query)
{
    Packet packet;
    packet.write(Connection::ServerCommands::QUERY, 1);
    packet.write(std::to_vector(query));
    this->write(packet);

    this->parser = std::make_shared<BufferParser>(this->readPackets());
    this->sequenceId = 0;
    return Response::build(*this->parser, this);
}

std::shared_ptr<Response> Connection::selectDatabase(const std::string &database)
{
    Packet packet;
    packet.write(Connection::ServerCommands::INIT_DB, 1);
    packet.write(std::to_vector(database));
    this->write(packet);

    this->parser = std::make_shared<BufferParser>(this->readPackets());
    this->sequenceId = 0;
    return Response::build(*this->parser, this);
}

std::shared_ptr<MysqlCpp::Responses::Response> Connection::ping()
{
    Packet packet;
    packet.write(Connection::ServerCommands::PING, 1);
    this->write(packet);

    this->parser = std::make_shared<BufferParser>(this->readPackets());
    this->sequenceId = 0;
    return Response::build(*this->parser, this);
}

void Connection::setDatabase(std::string database)
{
    this->database = database;
}

std::shared_ptr<Response> Connection::showTables()
{
    Packet packet;
    packet.write(Connection::ServerCommands::QUERY, 1);
    packet.write(std::to_vector("SHOW TABLES"));
    this->write(packet);

    this->parser = std::make_shared<BufferParser>(this->readPackets());
    auto response = Response::build(*this->parser, this);
    this->sequenceId = 0;
    return response;
}

std::shared_ptr<Response> Connection::createTable(std::string table)
{
    Packet packet;
    packet.write(Connection::ServerCommands::QUERY, 1);
    packet.write(std::to_vector(std::string("CREATE TABLE ").append(table).append(" (id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT, CONSTRAINT PRIMARY KEY (id))")));
    this->write(packet);

    BufferParser parser(this->readPackets());
    this->sequenceId = 0;
    return Response::build(parser, this);
}

bool Connection::capable(Connection::Capabilities capability) const
{
    return ((this->serverCapabilities & capability) & (this->clientCapabilities & capability)) > 0;
}

void Connection::setCapabilities(unsigned int capabilities)
{
    this->serverCapabilities = capabilities;
}

void Connection::setCapability(Connection::Capabilities capability, bool capable)
{
    this->serverCapabilities |= capable ? capability : ~capability;
}

void Connection::addSupportedAuthMethod(std::string authMethod)
{
    if (this->supportsAuthMethod(authMethod)) {
        return;
    }

    this->supportedAuthMethods.push_back(authMethod);
}

void Connection::removeSupportedAuthMethod(std::string authMethod)
{
    std::remove(this->supportedAuthMethods.begin(), this->supportedAuthMethods.end(), authMethod);
}

bool Connection::supportsAuthMethod(std::string authMethod)
{
    return std::find(this->supportedAuthMethods.begin(), this->supportedAuthMethods.end(), authMethod) != this->supportedAuthMethods.end();
}

void Connection::setClientAuthMethod(std::string authMethod)
{
    this->clientAuthMethod = authMethod;
}


bool Connection::hasStatus(Connection::ServerStatus serverStatus) const
{
    return (this->status & serverStatus) > 0;
}

std::vector<std::byte> Connection::generateCachingSha2AuthResponse(std::string password, std::string salt)
{
    if (salt.length() != SALT_LENGTH)
    {
        throw std::invalid_argument(std::string()
                                        .append("Password scramble data is invalid length (")
                                        .append(std::to_string(salt.length()))
                                        .append("). Expected ")
                                        .append(std::to_string(SALT_LENGTH)));
    }

    // Variables for processing
    byte digest1[SHA256_DIGEST_LENGTH];
    byte digest2[SHA256_DIGEST_LENGTH];
    byte digest3[SHA256_DIGEST_LENGTH];
    byte result[SHA256_DIGEST_LENGTH + 1];

    memset(result, 0, sizeof(result));

    SHA256_CTX sha256;
    // First SHA256
    {
        // Set up SHA256 hasher
        if (!SHA256_Init(&sha256))
        {
            throw "Unable to init SHA256 context in 1st SHA.";
        }

        if (!SHA256_Update(&sha256, (byte *)password.c_str(), password.size()))
        {
            throw "Unable to execute 1st SHA.";
        }

        if (!SHA256_Final(digest1, &sha256))
        {
            throw "Unable to save 1st SHA.";
        }
    }

    // Second SHA256
    {
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, digest1, SHA256_DIGEST_LENGTH);
        SHA256_Final(digest2, &sha256);
    }

    // Third SHA256 - with nonce
    {
        if (!SHA256_Init(&sha256))
        {
            throw "Unable to init SHA256 context in 3rd SHA.";
        }

        if (!SHA256_Update(&sha256, digest2, SHA256_DIGEST_LENGTH))
        {
            throw "Unable to execute 1st update of 3rd SHA.";
        }

        if (!SHA256_Update(&sha256, (byte *)salt.c_str(), salt.size()))
        {
            throw "Unable to executed 2nd update of 3rd SHA.";
        }

        if (!SHA256_Final(digest3, &sha256))
        {
            throw "Unable to save 3rd SHA.";
        }
    }

    digest3[SHA256_DIGEST_LENGTH] = '\0';

    // XOR into this->authResponse
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        result[i] = digest3[i] ^ digest1[i];
    }

    std::vector<std::byte> resultVector;
    resultVector.reserve(SHA256_DIGEST_LENGTH);
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        resultVector.push_back(std::byte(result[i]));
    }

    return resultVector;
}

std::vector<std::byte> Connection::rsaEncryptPassword(std::vector<std::byte> password, std::string publicKey)
{
    // Set up the RSA structs
    RSA *rsa;

    BIO *keyBio  = BIO_new_mem_buf(publicKey.c_str(), publicKey.length());
    if (keyBio == nullptr)
    {
        ERR_print_errors_fp(stderr);
        throw "Failed to create key BIO";
    }

    // Load the RSA structure
    rsa = PEM_read_bio_RSA_PUBKEY(keyBio, NULL, NULL, NULL);
    if (rsa == nullptr)
    {
        ERR_print_errors_fp(stderr);
        throw "Failed to read RSA public key";
    }

    std::vector<std::byte> encryptedPassword(RSA_size(rsa));
    // Encrypt the password
    auto encryptedLength = RSA_public_encrypt(password.size(), (unsigned char*)password.data(), (unsigned char*)encryptedPassword.data(), rsa, RSA_PKCS1_OAEP_PADDING);
    if (encryptedLength < 0) {
        throw "Failed to encrypt";
    }

    // Free up the RSA structs
    RSA_free(rsa);
    BIO_free(keyBio);

    return encryptedPassword;
}

std::string Connection::rsaEncryptPassword(std::string string, std::string publicKey)
{
    std::vector<std::byte> stringToEncrypt(string.size() + 1);
    std::transform(string.begin(), string.end(), stringToEncrypt.begin(), [](char c) { return std::byte(c); });
    auto encrypted = this->rsaEncryptPassword(stringToEncrypt, publicKey);

    std::string encryptedString(encrypted.size(), 0);
    std::transform(encrypted.begin(), encrypted.end(), encryptedString.begin(), [](std::byte b) { return (char)b; });
    return encryptedString;
}

std::vector<std::byte> Connection::rsaDecryptPassword(std::vector<std::byte> string, std::string privateKey)
{
    // Set up the RSA structs
    RSA *rsa = nullptr;

    BIO *keyBio  = BIO_new_mem_buf(privateKey.c_str(), privateKey.length());
    if (keyBio == nullptr)
    {
        ERR_print_errors_fp(stderr);
        throw "Failed to create key BIO";
    }

    // Load the RSA structure
    rsa = PEM_read_bio_RSAPrivateKey(keyBio, &rsa, NULL, NULL);
    if (rsa == nullptr)
    {
        ERR_print_errors_fp(stderr);
        throw "Failed to read RSA private key";
    }

    std::vector<std::byte> decrypted(RSA_size(rsa));
    int size = RSA_private_decrypt(RSA_size(rsa), reinterpret_cast<unsigned char*>(string.data()), reinterpret_cast<unsigned char*>(decrypted.data()), rsa, RSA_PKCS1_OAEP_PADDING);

    if (size < 0) {
        ERR_load_crypto_strings();
        while (auto errorCode = ERR_get_error()) {
            std::cout << "Error message: " << ERR_error_string(errorCode, nullptr) << std::endl;
        }
    }
    decrypted.resize(size);
    RSA_free(rsa);
    BIO_free(keyBio);

    return decrypted;
}

std::string Connection::rsaDecryptPassword(std::string encrypted, std::string privateKey)
{
    std::vector<std::byte> stringToDecrypt(encrypted.size());
    std::transform(encrypted.begin(), encrypted.end(), stringToDecrypt.begin(), [](char c) { return std::byte(c); });
    auto decrypted = this->rsaDecryptPassword(stringToDecrypt, privateKey);

    std::string decryptedString;
    std::transform(decrypted.begin(), decrypted.end(), decryptedString.begin(), [](std::byte b) { return (char)b; });
    return decryptedString;
}

void Connection::setStatus(unsigned int status)
{
    this->status = status;
}

unsigned int Connection::getPhase() const
{
    return this->phase;
}

const std::vector<char> Connection::readPackets()
{
    // Initial buffer
    std::string data(4, 0);

    // Read packet metadata
    if (((this->ssl == nullptr) ? read(this->socketHandle, &data[0], 4) : SSL_read(this->ssl, &data[0], 4)) < 0)
    {
        // TODO: Build logging system
        std::cerr << "Failed to read data from socket." << std::endl;
        return std::vector<char>();
    }

    auto packetParser = Support::BufferParser(std::vector<char>(data.begin(), data.end()));

    // Harvest payload length and sequence ID from metadata
    unsigned int payloadLength = packetParser.uInt<3>();
    (this->sequenceId = packetParser.uInt<1>())++; // Increment it for future writes

    // Resize buffer to fit payload
    data.resize(payloadLength);

    // Read payload from server based on payload length
    if (((this->ssl == nullptr) ? read(this->socketHandle, &data[0], payloadLength) : SSL_read(this->ssl, &data[0], payloadLength)) < 0)
    {
        // TODO: Build logging system
        // std::cerr << "Failed to read data from socket." << std::endl;
        return std::vector<char>();
    }

    // Return the read data. Recurse if the payload length is equal to (2^24) - 1
    std::vector<char> recursiveData = (payloadLength == ((unsigned int)0b111111111111111111111111) ? this->readPackets() : std::vector<char>());

    data.insert(data.end(), recursiveData.begin(), recursiveData.end());

    return std::vector<char>(data.begin(), data.end());
}

template <unsigned int x>
const int Connection::write(unsigned int data)
{
    unsigned int i = 0;
    int bytesSent = 0;
    while (i++ < x)
    {
        char byte = 0b11111111 & data;

        this->writeBuffer.push_back(byte);

        data >>= 8;
    }

    return bytesSent;
}

template <unsigned long long x>
const int Connection::write(const std::vector<std::byte> &data, bool nullTerminate)
{
    unsigned long long length = (x == 0 ? data.size() : x);

    std::for_each(data.begin(), data.begin() + length, [this](std::byte b) { this->write<1>((unsigned int)b); });

    if (nullTerminate)
    {
        this->write<1>(0);
    }

    return length;
}

const int Connection::write(Packet packet, bool resetSequenceId)
{
    std::vector<std::byte> payload = packet.data();
    packet.flush();
    unsigned int payloadLength = payload.size();
    unsigned int packetsWritten = 0;
    unsigned int bytesWritten = 0;

    while (payloadLength > 16777215U)
    {
        bytesWritten += this->write<3>(16777215U);
        bytesWritten += this->write<1>(this->sequenceId++);
        bytesWritten += this->write<16777215U>(payload);
        this->flushWriteBuffer();
        packetsWritten++;
        payload.erase(payload.begin(), payload.begin() + 16777215U);
        payloadLength = payload.size();
    }

    bytesWritten += this->write<3>(payloadLength);
    bytesWritten += this->write<1>(this->sequenceId++);
    bytesWritten += this->write(payload);
    this->flushWriteBuffer();

    if (resetSequenceId)
    {
        this->sequenceId = 0;
    }

    return packetsWritten + 1;
}

void Connection::flushWriteBuffer()
{
    char *data = new char[this->writeBuffer.size()];
    for (unsigned int i = 0; i < this->writeBuffer.size(); i++)
    {
        data[i] = (char)this->writeBuffer[i];
    }

    this->ssl == NULL
        ? ::send(this->socketHandle, data, this->writeBuffer.size(), 0)
        : SSL_write(this->ssl, data, this->writeBuffer.size());

    this->writeBuffer.clear();
}

void Connection::writeLengthEncoded(unsigned long long data)
{
    if (data >= 0 && data < 251)
    {
        this->write<1>(data);
        return;
    }

    if (data >= 251 && data < 65536) // 2^16
    {
        this->write<1>(0xFC);
        this->write<2>(data);
        return;
    }

    if (data >= 65536 && data < 16777216)
    {
        this->write<1>(0xFD);
        this->write<3>(data);
        return;
    }

    if (data >= 16777216 && data <= 18446744073709551615ULL)
    {
        this->write<1>(0xFE);
        this->write<8>(data);
        return;
    }
}

void Connection::writeLengthEncoded(std::vector<std::byte> data)
{
    this->writeLengthEncoded(data.size());
    this->write(data);
}

} // namespace MysqlCpp
