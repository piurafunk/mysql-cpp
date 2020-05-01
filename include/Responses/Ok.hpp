#pragma once

#include <MysqlCpp/Responses/Response.hpp>

#include <string>

namespace MysqlCpp
{

class Connection;

namespace Responses
{
class Ok : public Response
{
public:
    Ok(BufferParser &, Connection*);

    unsigned long getAffectedRows() const;
    unsigned long getLastInsertId() const;
    unsigned int getServerStatusFlags() const;
    unsigned int getWarningCount() const;
    std::string getInfo() const;
    std::string getSessionStateInfo() const;

protected:
    unsigned long affectedRows = 0;
    unsigned long lastInsertId = 0;
    unsigned int serverStatusFlags = 0;
    unsigned int warningCount = 0;
    std::string info;
    std::string sessionStateInfo;
};

} // namespace Responses
} // namespace MysqlCpp
