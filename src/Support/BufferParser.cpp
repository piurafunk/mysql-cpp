#include <MysqlCpp/Support/BufferParser.hpp>

#include <bitset>

namespace MysqlCpp::Support
{

BufferParser::BufferParser(std::vector<char> buffer)
{
    this->buffer = buffer;
}

unsigned int BufferParser::getCursor() const
{
    return this->cursor;
}

void BufferParser::seek(const unsigned int pos)
{
    this->cursor = pos;
}

void BufferParser::reset()
{
    this->cursor = 0;
}

bool BufferParser::hasNext() const
{
    return this->hasNext(this->cursor);
}

bool BufferParser::hasNext(const unsigned int &i) const
{
    return this->buffer.size() > i;
}

unsigned int BufferParser::uInt(unsigned int &i, unsigned int size) const
{
    unsigned int result = 0;
    unsigned int offset = 0;

    while (offset < size && i < this->buffer.size())
    {
        result |= (static_cast<unsigned char>(this->buffer[i++]) << (8 * offset++));
    }

    return result;
}

unsigned long BufferParser::uLong(unsigned int &i, unsigned int size) const
{
    unsigned long result = 0;
    unsigned int offset = 0;

    while (offset < size && i < this->buffer.size())
    {
        result |= (static_cast<unsigned char>(this->buffer[i++]) << (8 * offset++));
    }

    return result;
}

unsigned long BufferParser::lengthEncodedULong()
{
    return this->lengthEncodedULong(this->cursor);
}

unsigned long BufferParser::lengthEncodedULong(unsigned int &i) const
{
    auto length = this->uInt(i, 1);

    switch (length)
    {
    case 0xFC:
        return this->uInt(i, 2);
    case 0xFD:
        return this->uInt(i, 3);
    case 0xFE:
        return this->uLong(i, 8);
    default:
        return length;
    }
}

std::string BufferParser::string(unsigned int &i, const unsigned int size, bool nullTerminated) const
{
    std::string result;
    if (size == 0) // Null terminated strings
    {
        result = std::string(&this->buffer.at(i));

        // Move cursor past null terminated string
        i += result.length() + 1;
    }
    else // Fixed length strings
    {
        result = std::string(&this->buffer.at(i), size);

        i += size;
    }

    return result;
}

std::string BufferParser::lengthEncodedString()
{
    return this->lengthEncodedString(this->cursor);
}

std::string BufferParser::lengthEncodedString(unsigned int &i)
{
    auto length = this->lengthEncodedULong(i);

    return this->string(i, length);
}

std::string BufferParser::remainder()
{
    if (this->cursor >= this->buffer.size())
    {
        return std::string();
    }

    return this->string(this->cursor, this->length() - this->cursor);
}

const std::vector<char> &BufferParser::getBuffer() const
{
    return this->buffer;
}

const unsigned long BufferParser::length() const
{
    return this->buffer.size();
}

std::ostream &operator<<(std::ostream &out, const BufferParser &parser)
{
    unsigned int i = 0;
    while (parser.hasNext(i))
    {
        out << std::bitset<8>(parser.uInt(i, 1)) << std::endl;
    }

    return out;
}

} // namespace MysqlCpp::Support
