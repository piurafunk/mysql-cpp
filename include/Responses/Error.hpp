#pragma once

#include <MysqlCpp/Responses/Response.hpp>

#include <ostream>
#include <string>

namespace MysqlCpp
{

class Connection;

namespace Responses
{
class Error : public Response
{
public:
    Error(unsigned int, std::string);
    Error(BufferParser &, Connection *);

    unsigned int getErrorCode() const;
    std::string getSqlStateMarker() const;
    std::string getSqlState() const;
    std::string getErrorMessage() const;

protected:
    friend std::ostream &operator<<(std::ostream &, const Error &);
    unsigned int errorCode;
    std::string sqlStateMarker = "";
    std::string sqlState = "";
    std::string errorMessage;
};

} // namespace Responses
} // namespace MysqlCpp
