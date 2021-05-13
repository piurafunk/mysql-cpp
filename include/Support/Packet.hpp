#pragma once

#include <vector>

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
    void write(const std::vector<std::byte>&, bool = false);
    void write(std::byte);
    void writeLengthEncoded(unsigned long long);
    void writeLengthEncoded(std::vector<std::byte>, bool = false);

    void flush();
    std::vector<std::byte> data() const;

protected:
    std::vector<std::byte> buffer;
};

} // namespace MysqlCpp::Support
