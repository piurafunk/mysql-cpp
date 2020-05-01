#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/Eof.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

namespace MysqlCpp::Responses
{

Eof::Eof(BufferParser &parser, Connection *connection) : Response(parser, connection)
{
    this->warnings = parser.uInt<2>();
    this->statusFlags = parser.uInt<2>();
}

unsigned int Eof::getWarnings()
{
    return this->warnings;
}

unsigned int Eof::getStatusFlags()
{
    return this->statusFlags;
}

} // namespace MysqlCpp::Responses
