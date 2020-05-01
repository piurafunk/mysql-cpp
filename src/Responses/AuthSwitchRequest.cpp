#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/AuthSwitchRequest.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <iostream>
#include <bitset>

using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;

namespace MysqlCpp::Responses
{

AuthSwitchRequest::AuthSwitchRequest(BufferParser &parser, Connection *connection) : Response(parser, connection)
{
    this->pluginName = parser.string<0>();
    this->pluginData = parser.string<0>();
}

std::string AuthSwitchRequest::getPluginName() const
{
    return this->pluginName;
}

std::string AuthSwitchRequest::getPluginData() const
{
    return this->pluginData;
}

} // namespace MysqlCpp::Responses
