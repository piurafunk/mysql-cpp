#include <MysqlCpp/Responses/Response.hpp>

#include <string>

namespace MysqlCpp
{

class Connection;

namespace Responses
{

class AuthSwitchRequest : public Response
{
public:
    AuthSwitchRequest(BufferParser &, Connection *);

    std::string getPluginName() const;
    std::string getPluginData() const;
private:
    std::string pluginName;
    std::string pluginData;
};

} // namespace Responses
} // namespace MysqlCpp