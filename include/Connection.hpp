#pragma once

#include <openssl/ssl.h>

#include <memory>
#include <string>
#include <vector>

namespace MysqlCpp
{

namespace Responses
{
class Response;
}

namespace Support
{
class BufferParser;
class Packet;
} // namespace Support

class Connection
{
public:
    enum Capabilities : unsigned int
    {
        CLIENT_LONG_PASSWORD = 1UL << 0,
        CLIENT_FOUND_ROWS = 1UL << 1,
        CLIENT_LONG_FLAG = 1UL << 2,
        CLIENT_CONNECT_WITH_DB = 1UL << 3,
        CLIENT_NO_SCHEMA = 1UL << 4,
        CLIENT_COMPRESS = 1UL << 5,
        CLIENT_ODBC = 1UL << 6,
        CLIENT_LOCAL_FILES = 1UL << 7,
        CLIENT_IGNORE_SPACE = 1UL << 8,
        CLIENT_PROTOCOL_41 = 1UL << 9,
        CLIENIT_INTERACTIVE = 1UL << 10,
        CLIENT_SSL = 1UL << 11,
        CLIENT_IGNORE_SIGPIPE = 1UL << 12,
        CLIENT_TRANSACTIONS = 1UL << 13,
        CLIENT_RESERVED = 1UL << 14,
        CLIENT_RESERVED2 = 1UL << 15,
        CLIENT_MULTI_STATEMENTS = 1UL << 16,
        CLIENT_MULTI_RESULTS = 1UL << 17,
        CLIENT_PS_MULTI_RESULTS = 1UL << 18,
        CLIENT_PLUGIN_AUTH = 1UL << 19,
        CLIENT_CONNECT_ATTRS = 1UL << 20,
        CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA = 1UL << 21,
        CLIENIT_CAN_HANDLE_EXPIRED_PASSWORDS = 1UL << 22,
        CLIENT_SESSION_TRACK = 1UL << 23,
        CLIENT_DEPRECATE_EOF = 1UL << 24,
        CLIENT_OPTIONAL_RESULTSET_METADATA = 1UL << 25,
        CLIENT_ZSTD_COMPRESSSION_ALGORITHM = 1UL << 26,
        CLIENT_CAPABILITY_EXTENSION = 1UL << 29,
        CLIENT_REMEMBER_OPTIONS = 1UL << 31,
    };

    enum ErrorCodes : unsigned int
    {
        InvalidProtocolVersion = 1,
    };

    enum ServerStatus : unsigned int
    {
        SERVER_STATUS_IN_TRANS = 1U << 0,
        SERVER_STATUS_AUTOCOMMIT = 1U << 1,
        SERVER_MORE_RESULTS_EXIST = 1U << 3,
        SERVER_QUERY_NO_GOOD_INDEX_USED = 1U << 4,
        SERVER_QUERY_NO_INDEX_USED = 1U << 5,
        SERVER_STATUS_CURSOR_EXISTS = 1U << 6,
        SERVER_STATUS_LAST_ROW_SENT = 1U << 7,
        SERVER_STATUS_DB_DROPPED = 1U << 8,
        SERVER_STATUS_NO_BACKSLASH_ESCAPED = 1U << 9,
        SERVER_STATUS_METADATA_CHANGED = 1U << 10,
        SERVER_QUERY_WAS_SLOW = 1U << 11,
        SERVER_PS_OUT_PARAMS = 1U << 12,
        SERVER_STATUS_IN_TRANS_READONLY = 1U << 13,
        SERVER_SESSION_STATE_CHANGED = 1U << 14,
    };

    enum ServerCommands : unsigned int
    {
        QUIT = 0x01,
        INIT_DB = 0x02,
        QUERY = 0x03,
        CREATE_DB = 0x06,
        PING = 0x0E,
    };

    enum Phases : unsigned int
    {
        CONNECTION = 0x01,
        COMMAND = 0x02,
    };

    // Constructors
    Connection(std::string, unsigned int, std::string, std::string, bool = true);
    ~Connection();

    // Connection
    std::shared_ptr<MysqlCpp::Responses::Response> connect();
    std::shared_ptr<MysqlCpp::Responses::Response> disconnect();
    void receiveInitialHandshake();
    void sendSslRequest();
    void exchangeSsl();
    void sendHandshakeResponse();
    std::shared_ptr<MysqlCpp::Responses::Response> query(const std::string &);
    std::shared_ptr<MysqlCpp::Responses::Response> selectDatabase(const std::string &);
    std::shared_ptr<MysqlCpp::Responses::Response> ping();
    std::shared_ptr<MysqlCpp::Responses::Response> showTables();
    std::shared_ptr<MysqlCpp::Responses::Response> createTable(std::string);

    std::string generateSha2AuthResponse(std::string, std::string);
    std::vector<std::byte> rsaEncryptPassword(std::vector<std::byte>, std::string);
    std::vector<std::byte> rsaDecryptPassword(std::vector<std::byte>, std::string);
    void setDatabase(std::string);

    // Socket access
    const std::vector<char> readPackets();

    // Helper
    bool capable(Connection::Capabilities) const;
    bool hasStatus(Connection::ServerStatus) const;
    void setStatus(unsigned int);
    unsigned int getPhase() const;

protected:
    bool connected = false;
    std::string hostname;
    unsigned int port;
    std::string username;
    std::string password;
    std::string database;
    bool sslEnabled;
    SSL_CTX *sslCtx = NULL;
    SSL *ssl = NULL;
    unsigned int sequenceId = 0;
    std::vector<char> writeBuffer;
    unsigned int phase = 1;

    std::shared_ptr<Support::BufferParser> parser;

    unsigned int characterSet = 8;
    unsigned int clientCapabilities = Connection::Capabilities::CLIENT_PROTOCOL_41 | Connection::Capabilities::CLIENT_SSL | Connection::Capabilities::CLIENT_PLUGIN_AUTH | Connection::Capabilities::CLIENT_OPTIONAL_RESULTSET_METADATA | Connection::Capabilities::CLIENT_CONNECT_WITH_DB | Connection::Capabilities::CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA | Connection::Capabilities::CLIENT_DEPRECATE_EOF | Connection::Capabilities::CLIENT_TRANSACTIONS;
    unsigned int serverCapabilities = 0;
    unsigned int status = 0;
    std::string authPluginData;
    std::string authResponse = "";
    std::string clientAuthMethod = "caching_sha2_password";

    int socketHandle;

    template <unsigned int = 4>
    const int write(unsigned int);
    template <unsigned long long = 0>
    const int write(const std::string &, bool = false);
    const int write(MysqlCpp::Support::Packet, bool = true);
    void flushWriteBuffer();
    void writeLengthEncoded(unsigned long long);
    void writeLengthEncoded(std::string);
};

} // namespace MysqlCpp
