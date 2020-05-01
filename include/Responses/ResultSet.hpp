#pragma once

#include <MysqlCpp/Responses/Response.hpp>

#include <memory>
#include <vector>

namespace MysqlCpp
{

class Connection;

namespace Support
{
class BufferParser;
class ColumnDefinition;
class ResultRow;
} // namespace Support

namespace Responses
{
using MysqlCpp::Connection;
using MysqlCpp::Support::BufferParser;
using MysqlCpp::Support::ColumnDefinition;
using MysqlCpp::Support::ResultRow;

class ResultSet : public Response
{
public:
    ResultSet(BufferParser &, Connection *);
    std::vector<std::shared_ptr<ResultRow>> getRows() const;

protected:
    bool metadataPresent = false;
    unsigned int columnCount;
    std::vector<std::shared_ptr<ColumnDefinition>> columnDefinitions;
    std::vector<std::shared_ptr<ResultRow>> rows;
};

} // namespace Responses

} // namespace MysqlCpp
