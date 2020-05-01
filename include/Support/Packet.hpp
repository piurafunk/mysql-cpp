#pragma once

#include <string>
#include <sstream>

namespace MysqlCpp::Support
{

class Packet
{
public:
    Packet() {}
    Packet(const Packet &);

    void write(unsigned int, unsigned int);
    template <unsigned int = 4>
    void write(unsigned int);
    void write(const std::string, bool = false);
    void writeLengthEncoded(unsigned long long);
    void writeLengthEncoded(std::string, bool = false);

    std::string flush();
    std::string str() const;

protected:
    std::stringstream buffer;
};

} // namespace MysqlCpp::Support
