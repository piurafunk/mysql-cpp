#pragma once

#include <ostream>
#include <string>

namespace MysqlCpp
{

class Connection;

namespace Support
{

class BufferParser;

using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;

class ColumnDefinition
{
public:
    ColumnDefinition(BufferParser &, Connection *);

protected:
    friend std::ostream &operator<<(std::ostream &, const ColumnDefinition &);

    std::string catalog;
    std::string schema;
    std::string table;
    std::string orgTable;
    std::string name;
    std::string orgName;
    unsigned int length;
    unsigned int characterSet;
    unsigned int columnLength;
    unsigned int type;
    unsigned int flags;
    unsigned int decimals;
};

} // namespace Support

} // namespace MysqlCpp
