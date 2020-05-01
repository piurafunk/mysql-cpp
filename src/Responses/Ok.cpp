#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <iostream>
#include <bitset>

using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;

namespace MysqlCpp::Responses
{

Ok::Ok(BufferParser &parser, Connection *connection) : Response(parser, connection)
{
    this->affectedRows = parser.lengthEncodedULong();
    this->lastInsertId = parser.lengthEncodedULong();

    // Server status flags are next, regardless of support for CLIENT_PROTOCOL_41
    connection->setStatus(this->serverStatusFlags = parser.uInt<2>());

    if (connection->capable(Connection::Capabilities::CLIENT_PROTOCOL_41))
    {
        // Normally the server status flags would be pulled here, but we've already pulled them a few lines up
        this->warningCount = parser.uInt<2>();
    }
    // Normally the else would pull only server status flags, but we've already pulled them a few lines up

    if (connection->capable(Connection::Capabilities::CLIENT_SESSION_TRACK))
    {
        this->info = parser.lengthEncodedString();

        if (connection->hasStatus(Connection::ServerStatus::SERVER_SESSION_STATE_CHANGED))
        {
            this->sessionStateInfo = parser.lengthEncodedString();
        }
    }
    else
    {
        this->info = parser.remainder();
    }
}

unsigned long Ok::getAffectedRows() const
{
    return this->affectedRows;
}

unsigned long Ok::getLastInsertId() const
{
    return this->lastInsertId;
}

unsigned int Ok::getServerStatusFlags() const
{
    return this->serverStatusFlags;
}

unsigned int Ok::getWarningCount() const
{
    return this->warningCount;
}

std::string Ok::getInfo() const
{
    return this->info;
}

std::string Ok::getSessionStateInfo() const
{
    return this->sessionStateInfo;
}

} // namespace MysqlCpp::Responses
