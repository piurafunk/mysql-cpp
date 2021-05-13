#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>
#include <MysqlCpp/Responses/Ok.hpp>

#include <iostream>
#include <bitset>

SCENARIO("A connection can only use \"sha256_password\" auth method", "[connection/sha256]")
{
    GIVEN("A connection with only \"sha256_password\" auth method")
    {
        auto connection = std::make_shared<MysqlCpp::Connection>("mysql", 3306, "user", "password", false);
        connection->setClientAuthMethod("caching_sha2_password");

        WHEN("The connection connects")
        {
            auto response = connection->connect();

            THEN("The response is an instance of MysqlCpp::Responses::Ok")
            {
                REQUIRE(::instanceOf<MysqlCpp::Responses::Ok>(response) == true);
            }

            connection->disconnect();
        }
    }
}
