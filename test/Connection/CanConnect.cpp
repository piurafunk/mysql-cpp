#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>
#include <MysqlCpp/Responses/Error.hpp>
#include <MysqlCpp/Responses/Ok.hpp>
#include <MysqlCpp/Responses/Response.hpp>
#include <MysqlCpp/Responses/ResultSet.hpp>

#include <iostream>

TEST_CASE("Connector can communicate", "[connection]")
{
    // Consts to confirm equality
    const std::string hostname = "mysql";
    const unsigned int port = 3306;
    const std::string username = "user";
    const std::string password = "password";
    const std::string database = "test_database";

    // Tests
    auto connection = std::make_shared<MysqlCpp::Connection>(hostname, port, username, password, false);
    std::cout << "Beginning connection" << std::endl;
    auto result = connection->connect();

    REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(result) == true);

    std::cout << "Pinging" << std::endl;
    result = connection->ping();
    REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(result) == true);

    SECTION("select database")
    {
        // Create database
        std::cout << "Beginning create database" << std::endl;
        auto createDatabaseResponse = connection->query("CREATE DATABASE `test_database`");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(createDatabaseResponse) == true);

        // Select database
        std::cout << "Beginning select database" << std::endl;
        auto selectDatabaseResponse = connection->selectDatabase("test_database");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);
    }

    SECTION("show tables")
    {
        // Select database
        std::cout << "Beginning select database" << std::endl;
        auto selectDatabaseResponse = connection->selectDatabase("test_database");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);

        // Show tables
        std::cout << "Beginning show tables" << std::endl;
        auto showTablesResponse = connection->showTables();
        REQUIRE(::instanceOf<MysqlCpp::Responses::ResultSet>(showTablesResponse) == true);
        auto resultSet = dynamic_pointer_cast<MysqlCpp::Responses::ResultSet>(showTablesResponse);
        REQUIRE(resultSet->getRows().size() == 0);
    }

    SECTION("create table")
    {
        // Select database
        std::cout << "Beginning select database" << std::endl;
        auto selectDatabaseResponse = connection->selectDatabase("test_database");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(selectDatabaseResponse) == true);

        // Create table
        std::cout << "Beginning create table" << std::endl;
        auto createTableResponse = connection->createTable("test_table");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(createTableResponse) == true);
    }

    SECTION("drop database")
    {
        std::cout << "Beginning drop database" << std::endl;
        // Delete database
        auto dropDatabaseResponse = connection->query("DROP DATABASE test_database");
        REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(dropDatabaseResponse) == true);
    }

    std::cout << "Disconnecting" << std::endl;
    auto disconnectResponse = connection->disconnect();

    REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(disconnectResponse) == true);
}
