#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <memory>

using MysqlCpp::Support::BufferParser;

TEST_CASE("BufferParser can parse fixed length integer", "[Support/BufferParser - FixedLengthInteger]")
{
    auto parser = std::make_shared<BufferParser>(std::vector<char>({0b1, 0b10, 0b11, 0b100}));
    auto i = 0U;

    SECTION("int<1>")
    {
        auto result = parser->uInt(i, 1);

        REQUIRE(result == 0b1);
    }

    SECTION("int<2>")
    {
        auto result = parser->uInt(i, 2);

        REQUIRE(result == 0b10'00000001);
    }

    SECTION("int<3>")
    {
        auto result = parser->uInt(i, 3);

        REQUIRE(result == 0b11'00000010'00000001);
    }

    SECTION("int<4>")
    {
        auto result = parser->uInt(i, 4);

        REQUIRE(result == 0b100'00000011'00000010'00000001);
    }

    SECTION("int<1> internal")
    {
        auto result = parser->uInt<1>();

        REQUIRE(result == 0b1);
    }

    SECTION("int<2> internal")
    {
        auto result = parser->uInt<2>();

        REQUIRE(result == 0b10'00000001);
    }

    SECTION("int<3> internal")
    {
        auto result = parser->uInt<3>();

        REQUIRE(result == 0b11'00000010'00000001);
    }

    SECTION("int<4> internal")
    {
        auto result = parser->uInt<4>();

        REQUIRE(result == 0b100'00000011'00000010'00000001);
    }
}
