#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Responses/Response.hpp>
#include <MysqlCpp/Responses/ResultSet.hpp>

#include <iostream>

SCENARIO("Connector can communicate", "[connection]")
{
    GIVEN("A connection with valid details")
    {
        // Consts to confirm equality
        const std::string hostname = "mysql";
        const unsigned int port = 3306;
        const std::string username = "user";
        const std::string password = "password";
        const std::string database = "test_database";

        auto connection = std::make_shared<MysqlCpp::Connection>(hostname, port, username, password, false);
        auto result = connection->connect();

        WHEN("Connected")
        {
            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(result) == true);
            }
        }

        WHEN("Pinging")
        {
            result = connection->ping();

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(result) == true);
            }
        }

        WHEN("Creating a database")
        {
            auto createDatabaseResponse = connection->query("CREATE DATABASE `test_database`");

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(createDatabaseResponse) == true);
            }
        }

        WHEN("Selecting a database")
        {
            auto selectDatabaseResponse = connection->selectDatabase("test_database");

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);
            }
        }

        WHEN("Querying \"show tables\"")
        {
            auto selectDatabaseResponse = connection->selectDatabase("test_database");
            REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);

            auto showTablesResponse = connection->showTables();
            THEN("The response is an instance of MysqlCpp::Responses::ResultSet")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::ResultSet>(showTablesResponse) == true);
            }

            auto resultSet = std::dynamic_pointer_cast<MysqlCpp::Responses::ResultSet>(showTablesResponse);

            THEN("The result set has 0 rows")
            {
                REQUIRE(resultSet->getRows().size() == 0);
            }
        }

        WHEN("Creating a table")
        {
            auto selectDatabaseResponse = connection->selectDatabase("test_database");
            REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);

            auto createTableResponse = connection->createTable("test_table");

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(createTableResponse) == true);
            }
        }

        WHEN("Dropping a database")
        {
            auto dropDatabaseResponse = connection->query("DROP DATABASE test_database");

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(dropDatabaseResponse) == true);
            }
        }

        WHEN("Disconnecting")
        {
            auto disconnectResponse = connection->disconnect();

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(disconnectResponse) == true);
            }
        }
    }
}
