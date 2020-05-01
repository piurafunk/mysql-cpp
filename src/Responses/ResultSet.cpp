#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/Responses/Eof.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Responses/ResultSet.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>
#include <MysqlCpp/Support/ColumnDefinition.hpp>
#include <MysqlCpp/Support/ResultRow.hpp>

#include <bitset>
#include <iostream>

using MysqlCpp::Connection;
using MysqlCpp::Responses::Eof;
using MysqlCpp::Responses::Error;
using MysqlCpp::Responses::Ok;
using MysqlCpp::Support::BufferParser;
using MysqlCpp::Support::ColumnDefinition;

namespace MysqlCpp::Responses
{

ResultSet::ResultSet(MysqlCpp::Support::BufferParser &parser, MysqlCpp::Connection *connection)
{
    std::cout << "Data:" << std::endl
              << parser << std::endl;
    if (connection->capable(Connection::Capabilities::CLIENT_OPTIONAL_RESULTSET_METADATA))
    {
        std::cout << "Checking for metadata flag" << std::endl;
        this->metadataPresent = (parser.uInt<1>() > 0);
    }

    this->columnCount = parser.lengthEncodedULong();
    std::cout << "Column count: " << this->columnCount << std::endl;

    if (!connection->capable(Connection::Capabilities::CLIENT_OPTIONAL_RESULTSET_METADATA) || this->metadataPresent)
    {
        std::cout << "Metadata is present" << std::endl;
        // Harvest column definitions
        this->columnDefinitions.resize(this->columnCount);
        for (unsigned int i = 0; i < this->columnCount; i++)
        {
            parser = BufferParser(connection->readPackets());

            std::cout << "Building column definition " << i << std::endl;
            this->columnDefinitions[i] = std::make_shared<ColumnDefinition>(parser, connection);
        }

        std::cout << "Done harvesting metadata" << std::endl
                  << std::endl;
    }

    if (!connection->capable(Connection::Capabilities::CLIENT_DEPRECATE_EOF))
    {
        parser = BufferParser(connection->readPackets());
        std::make_shared<Eof>(parser, connection);
        // We don't really do anything with this
    }

    std::cout << "Column definitions:" << std::endl
              << std::endl;
    for (std::shared_ptr<ColumnDefinition> columnDefinition : this->columnDefinitions)
    {
        std::cout << *columnDefinition.get() << std::endl;
    }

    do
    {
        parser = BufferParser(connection->readPackets());
        unsigned int nextByte = parser.uInt<1>();
        parser.reset();

        std::cout << "Byte: " << std::bitset<8>(nextByte) << std::endl;

        // Check for error packet
        if (nextByte == 0xFF)
        {
            auto errorPacket = dynamic_pointer_cast<Error>(Response::build(parser, connection));
            std::cout << "Error message: " << errorPacket->getErrorMessage() << std::endl;
            return;
        }

        // Check for OK packet
        if (connection->capable(Connection::Capabilities::CLIENT_DEPRECATE_EOF) && nextByte == 0x00)
        {
            // OK packet
            Response::build(parser, connection);
            break;
        }
        else if (nextByte == 0xFE)
        {
            // EOF packet
            Response::build(parser, connection);
            break;
        }

        std::cout << "Building colunn..." << std::endl;
        this->rows.push_back(std::make_shared<ResultRow>(parser, this->columnCount));
    } while (true);
}

std::vector<std::shared_ptr<ResultRow>> ResultSet::getRows() const
{
    return this->rows;
}

} // namespace MysqlCpp::Responses
