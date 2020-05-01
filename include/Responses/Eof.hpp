#pragma once

#include <MysqlCpp/Responses/Response.hpp>

namespace MysqlCpp
{

class Connection;

namespace Support
{
class BufferParser;
}

namespace Responses
{

class Eof : public Response
{
public:
    Eof(BufferParser &, Connection *);

    unsigned int getWarnings();
    unsigned int getStatusFlags();

protected:
    unsigned int warnings;
    unsigned int statusFlags;
};

} // namespace Responses

} // namespace MysqlCpp
