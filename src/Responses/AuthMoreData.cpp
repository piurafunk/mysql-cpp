#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/AuthMoreData.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <iostream>
#include <bitset>

using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;

namespace MysqlCpp::Responses
{

AuthMoreData::AuthMoreData(BufferParser &parser, Connection *connection) : Response(parser, connection)
{
    this->data = parser.remainder();
}

std::string AuthMoreData::getData() const
{
    return this->data;
}

} // namespace MysqlCpp::Responses
