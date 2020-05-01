#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace MysqlCpp::Support
{

class BufferParser
{
public:
    BufferParser(std::vector<char>);

    unsigned int getCursor() const;
    void seek(const unsigned int);
    void reset();
    bool hasNext() const;
    bool hasNext(const unsigned int &) const;

    template <unsigned int>
    unsigned int uInt();
    unsigned int uInt(unsigned int &, unsigned int) const;
    template <unsigned int>
    unsigned long uLong();
    unsigned long uLong(unsigned int &, unsigned int) const;
    unsigned long lengthEncodedULong();
    unsigned long lengthEncodedULong(unsigned int &) const;

    template <unsigned int>
    std::string string(bool = true);
    std::string string(unsigned int &, const unsigned int = 0, bool = true) const;
    std::string lengthEncodedString();
    std::string lengthEncodedString(unsigned int &);

    std::string remainder();

    const std::vector<char> &getBuffer() const;
    const unsigned long length() const;

protected:
    friend std::ostream &operator<<(std::ostream &, const BufferParser &);

    std::vector<char> buffer;
    unsigned int cursor = 0;
};

template <unsigned int size>
unsigned int BufferParser::uInt()
{
    return this->uInt(this->cursor, size);
}

template <unsigned int size>
unsigned long BufferParser::uLong()
{
    return this->uLong(this->cursor, size);
}

template <unsigned int size>
std::string BufferParser::string(bool nullTerminated)
{
    return this->string(this->cursor, size, nullTerminated);
}

} // namespace MysqlCpp::Support
