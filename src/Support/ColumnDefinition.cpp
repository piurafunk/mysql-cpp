#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <MysqlCpp/Support/ColumnDefinition.hpp>

#include <iostream>

namespace MysqlCpp::Support
{

ColumnDefinition::ColumnDefinition(BufferParser &parser, Connection *connection)
{
    if (connection->capable(Connection::Capabilities::CLIENT_PROTOCOL_41))
    {
        std::cout << "Parsing catalog" << std::endl;
        this->catalog = parser.lengthEncodedString();
        std::cout << "Parsing schema" << std::endl;
        this->schema = parser.lengthEncodedString();
        std::cout << "Parsing table" << std::endl;
        this->table = parser.lengthEncodedString();
        this->orgTable = parser.lengthEncodedString();
        this->name = parser.lengthEncodedString();
        this->orgName = parser.lengthEncodedString();
        this->length = parser.lengthEncodedULong();
        this->characterSet = parser.uInt<2>();
        this->columnLength = parser.uInt<4>();
        this->type = parser.uInt<1>();
        this->flags = parser.uInt<2>();
        this->decimals = parser.uInt<1>();
    }
    else
    {
        // Implement column definition 320
    }
}

std::ostream &operator<<(std::ostream &out, const ColumnDefinition &columnDefinition)
{
    return out << "Catalog: " << columnDefinition.catalog << std::endl
               << "Schema: " << columnDefinition.schema << std::endl
               << "Table: " << columnDefinition.table << std::endl
               << "Org table: " << columnDefinition.orgTable << std::endl
               << "Name: " << columnDefinition.name << std::endl
               << "Org name: " << columnDefinition.orgName << std::endl
               << "Length: " << columnDefinition.length << std::endl
               << "Character set: " << columnDefinition.characterSet << std::endl
               << "Column length: " << columnDefinition.columnLength << std::endl
               << "Type: " << columnDefinition.type << std::endl
               << "Flags: " << columnDefinition.flags << std::endl
               << "Decimals: " << columnDefinition.decimals << std::endl;
}

} // namespace MysqlCpp::Support
