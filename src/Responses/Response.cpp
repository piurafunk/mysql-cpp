#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/AuthMoreData.hpp>
#include <MysqlCpp/Responses/AuthSwitchRequest.hpp>
#include <MysqlCpp/Responses/Eof.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Responses/Response.hpp>
#include <MysqlCpp/Responses/ResultSet.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <bitset>
#include <iostream>

using MysqlCpp::Connection;

namespace MysqlCpp::Responses
{

Response::Response()
{
}

Response::Response(BufferParser &parser, Connection *connection)
{
    this->header = parser.uInt<1>();
}

std::shared_ptr<Response> Response::build(BufferParser &parser, Connection *connection)
{
    std::cout << "Building from: " << std::endl
              << parser << std::endl;
    unsigned int i = 0;
    unsigned int header = parser.uInt(i, 1);

    // Check for disconnect response packet (no data)
    if (parser.length() == 0)
    {
        std::cout << "Creating OK packet for disconnect" << std::endl;
        return std::make_shared<Ok>(parser, connection);
    }

    // Check for EOF packet
    if (header == 0xFE && parser.length() < 9)
    {
        std::cout << "Creating EOF packet" << std::endl;
        return std::make_shared<Eof>(parser, connection);
    }

    // Check for OK packet
    if (header == 0x00 && parser.length() >= 7)
    {
        std::cout << "Creating OK packet" << std::endl;
        return std::make_shared<Ok>(parser, connection);
    }

    if (connection->getPhase() == Connection::Phases::CONNECTION)
    {
        switch (header)
        {
        case 0x01:
            std::cout << "Creating AuthMoreData packet" << std::endl;
            return std::make_shared<AuthMoreData>(parser, connection);
        case 0xFE: // Auth switch request packet
            std::cout << "Creating AuthSwitchRequest packet" << std::endl;
            return std::make_shared<AuthSwitchRequest>(parser, connection);
        case 0xFB: // More data
            std::cout << "Creating more data packet" << std::endl;
            break;
        case 0xFF: // Error packet
            std::cout << "Creating ERR packet" << std::endl;
            return std::make_shared<Error>(parser, connection);
        default: // Text result set
            std::cout << "Creating default packet" << std::endl;
            return std::make_shared<ResultSet>(parser, connection);
        }
    }
    else
    {
        switch (header)
        {
        case 0x00: // OK packet
            std::cout << "Creating OK packet" << std::endl;
            return std::make_shared<Ok>(parser, connection);
        case 0xFB: // More data
            std::cout << "Creating more data packet" << std::endl;
            break;
        case 0xFE: // EOF packet
            std::cout << "Creating EOF packet" << std::endl;
            return std::make_shared<Eof>(parser, connection);
        case 0xFF: // Error packet
            std::cout << "Creating ERR packet" << std::endl;
            return std::make_shared<Error>(parser, connection);
        default: // Text result set
            std::cout << "Creating default packet" << std::endl;
            return std::make_shared<ResultSet>(parser, connection);
        }
    }
} // namespace MysqlCpp::Responses

unsigned int Response::getHeader() const
{
    return this->header;
}

} // namespace MysqlCpp::Responses
