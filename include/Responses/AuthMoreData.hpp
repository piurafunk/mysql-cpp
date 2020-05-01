#include <MysqlCpp/Responses/Response.hpp>

#include <string>

namespace MysqlCpp
{

class Connection;

namespace Responses
{

class AuthMoreData : public Response
{
public:
    AuthMoreData(BufferParser &, Connection *);

    std::string getData() const;

private:
    std::string data;
};

} // namespace Responses
} // namespace MysqlCpp