#pragma once

#include <memory>

namespace MysqlCpp
{
class Connection;
namespace Support
{
class BufferParser;
}

using MysqlCpp::Support::BufferParser;

namespace Responses
{

class Response
{
public:
    Response();
    Response(BufferParser &, Connection *);
    virtual ~Response() = default;

    static std::shared_ptr<Response> build(BufferParser &, Connection *);

    unsigned int getHeader() const;

protected:
    unsigned int header;
};

} // namespace Responses
} // namespace MysqlCpp
