#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <iostream>

using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;

namespace MysqlCpp
{
namespace Responses
{

Error::Error(unsigned int code, std::string message)
{
    this->errorCode = code;
    this->errorMessage = message;
}

Error::Error(BufferParser &parser, Connection *connection) : Response(parser, connection)
{
    this->errorCode = parser.uInt<2>();

    if (connection->capable(Connection::Capabilities::CLIENT_PROTOCOL_41))
    {
        this->sqlStateMarker = parser.string<1>();
        this->sqlState = parser.string<5>();
    }

    this->errorMessage = parser.remainder();

    std::cout << "ERROR OCCURRED!!" << std::endl
              << std::endl
              << *this << std::endl;
}

unsigned int Error::getErrorCode() const
{
    return this->errorCode;
}

std::string Error::getSqlStateMarker() const
{
    return this->sqlStateMarker;
}

std::string Error::getSqlState() const
{
    return this->sqlState;
}

std::string Error::getErrorMessage() const
{
    return this->errorMessage;
}

std::ostream &operator<<(std::ostream &out, const Error &error)
{
    out << "Error code: " << error.getErrorCode() << std::endl
        << "Sql state marker: " << error.getSqlStateMarker() << std::endl
        << "Sql state: " << error.getSqlState() << std::endl
        << "Error message: " << error.getErrorMessage() << std::endl;

    return out;
}

} // namespace Responses
} // namespace MysqlCpp
