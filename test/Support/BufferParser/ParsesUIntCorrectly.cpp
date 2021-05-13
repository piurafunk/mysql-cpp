#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <memory>

using MysqlCpp::Support::BufferParser;

SCENARIO("BufferParser can parse fixed length integer", "[Support/BufferParser - FixedLengthInteger]")
{
    GIVEN("A buffer of basic integers")
    {
        auto parser = std::make_shared<BufferParser>(std::vector<char>({0b1, 0b10, 0b11, 0b100}));
        auto i = 0U;

        WHEN("Fetching the first byte as an integer")
        {
            auto result = parser->uInt(i, 1);

            THEN("The value equals 0b1")
            {
                REQUIRE(result == 0b1);
            }
        }

        WHEN("Fetching the first 2 bytes as an integer")
        {
            auto result = parser->uInt(i, 2);

            THEN("The value equals 0b10'00000001")
            {
                REQUIRE(result == 0b10'00000001);
            }
        }

        WHEN("Fetching the first 3 bytes as an integer")
        {
            auto result = parser->uInt(i, 3);

            THEN("The value equals 0b11'00000010'00000001")
            {
                REQUIRE(result == 0b11'00000010'00000001);
            }
        }

        WHEN("Fetching the first 4 bytes as an integer")
        {
            auto result = parser->uInt(i, 4);

            THEN("The value equals 0b100'00000011'00000010'00000001")
            {
                REQUIRE(result == 0b100'00000011'00000010'00000001);
            }
        }

        WHEN("Fetching the first byte as an integer using the \"internal\" function")
        {
            auto result = parser->uInt<1>();

            THEN("The value equals 0b1")
            {
                REQUIRE(result == 0b1);
            }
        }

        WHEN("Fetching the first 2 bytes as an integer using the \"internal\" function")
        {
            auto result = parser->uInt<2>();

            THEN("The value equals 0b10'00000001")
            {
                REQUIRE(result == 0b10'00000001);
            }
        }

        WHEN("Fetching the first 3 bytes as an integer using the \"internal\" function")
        {
            auto result = parser->uInt<3>();

            THEN("The value equals 0b11'00000010'00000001")
            {
                REQUIRE(result == 0b11'00000010'00000001);
            }
        }

        SECTION("Fetching the first 4 bytes as an integer using the \"internal\" function")
        {
            auto result = parser->uInt<4>();

            THEN("The value equals 0b100'00000011'00000010'00000001")
            {
                REQUIRE(result == 0b100'00000011'00000010'00000001);
            }
        }
    }
}
