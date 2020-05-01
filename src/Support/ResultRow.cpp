#include <MysqlCpp/Support/ResultRow.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

using MysqlCpp::Support::BufferParser;

namespace MysqlCpp::Support
{

ResultRow::ResultRow(BufferParser &parser, unsigned int columnCount)
{
    this->columns.resize(columnCount);

    for (unsigned int i = 0; i < columnCount; i++)
    {
        const unsigned int cursor = parser.getCursor();
        unsigned int nextByte = parser.uInt<1>();
        parser.seek(cursor);

        if (nextByte = 0xFB)
        {
            this->columns.push_back(std::string());
        }
        else
        {
            this->columns.push_back(parser.lengthEncodedString());
        }
    }
}

} // namespace MysqlCpp::Support
