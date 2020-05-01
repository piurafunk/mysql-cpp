#pragma once

#include <string>
#include <vector>

namespace MysqlCpp
{

namespace Support
{

class BufferParser;

using MysqlCpp::Support::BufferParser;

class ResultRow
{
public:
    ResultRow(BufferParser &, unsigned int);

protected:
    std::vector<std::string> columns;
};

} // namespace Support

} // namespace MysqlCpp
