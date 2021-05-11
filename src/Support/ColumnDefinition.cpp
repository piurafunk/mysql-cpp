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
        std::cout << "Parsing orgTable" << std::endl;
        this->orgTable = parser.lengthEncodedString();
        std::cout << "Parsing name" << std::endl;
        this->name = parser.lengthEncodedString();
        std::cout << "Parsing orgName" << std::endl;
        this->orgName = parser.lengthEncodedString();
        std::cout << "Parsing length" << std::endl;
        this->length = parser.lengthEncodedULong();
        std::cout << "Parsing characterSet" << std::endl;
        this->characterSet = parser.uInt<2>();
        std::cout << "Parsing columnLength" << std::endl;
        this->columnLength = parser.uInt<4>();
        std::cout << "Parsing type" << std::endl;
        this->type = parser.uInt<1>();
        std::cout << "Parsing flags" << std::endl;
        this->flags = parser.uInt<2>();
        std::cout << "Parsing decimals" << std::endl;
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
