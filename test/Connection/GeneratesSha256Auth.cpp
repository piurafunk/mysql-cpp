#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>

#include <iostream>

TEST_CASE("Connector can generate SHA 256 hashes", "[connection-auth]")
{
    const std::string hostname = "mysql";
    const unsigned int port = 3306;
    const std::string username = "user";
    const std::string password = "password";
    const std::string database = "test_database";

    auto connection = std::make_shared<MysqlCpp::Connection>(hostname, port, username, password, false);

    // Tests
    SECTION("Password: password")
    {
        std::string password = "\x70\x61\x73\x73\x77\x6f\x72\x64";
        std::string salt = "\x42\x78\x5a\x3f\x0c\x62\x13\x43\x6e\x11\x6a\x45\x6b\x38\x75\x22\x14\x59\x15\x01";
        std::string expected = "\x13\x69\x4f\xcc\x48\xf8\x4c\xa1\xb6\xa1\x1f\xd2\x8a\x34\x8b\x92\x12\x25\x33\x55\x57\xba\xb1\x6a\x66\xfb\x30\x5e\xf7\xad\x42\x5b";

        std::string authResponse = connection->generateSha2AuthResponse(password, salt);

        REQUIRE(authResponse == expected);
    }
}
