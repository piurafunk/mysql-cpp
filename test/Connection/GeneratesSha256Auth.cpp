#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>
#include <MysqlCpp/helpers.hpp>

#include <iostream>

SCENARIO("Connector can generate SHA 256 hashes", "[connection-auth]")
{
    GIVEN("A connection")
    {
        auto connection = std::make_shared<MysqlCpp::Connection>("", 0, "", "");

        WHEN("Generating a cached SHA2 auth response")
        {
            std::string password = "\x70\x61\x73\x73\x77\x6f\x72\x64";
            std::string salt = "\x42\x78\x5a\x3f\x0c\x62\x13\x43\x6e\x11\x6a\x45\x6b\x38\x75\x22\x14\x59\x15\x01";
            std::vector<std::byte> expected = {
                std::byte(0x13),
                std::byte(0x69),
                std::byte(0x4f),
                std::byte(0xcc),
                std::byte(0x48),
                std::byte(0xf8),
                std::byte(0x4c),
                std::byte(0xa1),
                std::byte(0xb6),
                std::byte(0xa1),
                std::byte(0x1f),
                std::byte(0xd2),
                std::byte(0x8a),
                std::byte(0x34),
                std::byte(0x8b),
                std::byte(0x92),
                std::byte(0x12),
                std::byte(0x25),
                std::byte(0x33),
                std::byte(0x55),
                std::byte(0x57),
                std::byte(0xba),
                std::byte(0xb1),
                std::byte(0x6a),
                std::byte(0x66),
                std::byte(0xfb),
                std::byte(0x30),
                std::byte(0x5e),
                std::byte(0xf7),
                std::byte(0xad),
                std::byte(0x42),
                std::byte(0x5b),
            };

            std::vector<std::byte> authResponse = connection->generateCachingSha2AuthResponse(password, salt);

            THEN("The generated response equals the expected")
            {
                REQUIRE(authResponse == expected);
            }
        }
    }
}
