#include <MysqlCpp/Support/Packet.hpp>
#include <MysqlCpp/helpers.hpp>

#include <iostream>

namespace MysqlCpp::Support
{

Packet::Packet(const Packet &packet)
{
    this->buffer = packet.data();
}

// template <unsigned int x>
void Packet::write(unsigned int data, unsigned int x)
{
    unsigned int i = 0;
    while (i++ < x)
    {
        this->buffer.push_back(std::byte(data));

        data >>= 8;
    }
}

void Packet::write(const std::vector<std::byte> &data, bool nullTerminate)
{
    this->buffer.reserve(this->buffer.size() + data.size());
    this->buffer.insert(this->buffer.end(), data.begin(), data.end());

    if (nullTerminate)
    {
        this->write(0, 1);
    }
}

void Packet::write(std::byte byte)
{
    this->buffer.push_back(byte);
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

void Packet::writeLengthEncoded(std::vector<std::byte> data, bool nullTerminated)
{
    this->writeLengthEncoded(data.size());
    this->write(data, nullTerminated);
}

void Packet::flush()
{
    this->buffer.resize(0);
}

std::vector<std::byte> Packet::data() const
{
    return this->buffer;
}

} // namespace MysqlCpp::Support
