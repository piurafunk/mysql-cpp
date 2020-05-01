#include <MysqlCpp/Support/Packet.hpp>

#include <iostream>

namespace MysqlCpp::Support
{

Packet::Packet(const Packet &packet)
{
    this->buffer << packet.str();
}

// template <unsigned int x>
void Packet::write(unsigned int data, unsigned int x)
{
    unsigned int i = 0;
    while (i++ < x)
    {
        this->buffer << (uint8_t)(0b11111111 & data);

        data >>= 8;
    }
}

void Packet::write(const std::string data, bool nullTerminate)
{
    this->buffer << data;

    if (nullTerminate)
    {
        this->write(0, 1);
    }
}

void Packet::writeLengthEncoded(unsigned long long data)
{
    if (data >= 0 && data < 251)
    {
        this->write(data, 1);
        return;
    }

    if (data >= 251 && data < 65536) // 2^16
    {
        this->write(0xFC, 1);
        this->write(data, 2);
        return;
    }

    if (data >= 65536 && data < 16777216)
    {
        this->write(0xFD, 1);
        this->write(data, 3);
        return;
    }

    if (data >= 16777216 && data <= 18446744073709551615ULL)
    {
        this->write(0xFE, 1);
        this->write(data, 8);
        return;
    }
}

void Packet::writeLengthEncoded(std::string data, bool nullTerminated)
{
    this->writeLengthEncoded(data.length());
    this->write(data, nullTerminated);
}

std::string Packet::flush()
{
    std::string temp = this->buffer.str();
    this->buffer.str("");
    return temp;
}

std::string Packet::str() const
{
    return this->buffer.str();
}

} // namespace MysqlCpp::Support
