#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <memory>

using MysqlCpp::Support::BufferParser;

TEST_CASE("BufferParser knows when it has next", "[Support/BufferParser - hasNext]")
{
    auto parser = std::make_shared<BufferParser>(std::vector<char>({0b1, 0b10, 0b11, 0b100}));

    SECTION("cursor = 0")
    {
        REQUIRE(parser->hasNext() == true);
    }

    SECTION("cursor = 1")
    {
        parser->uInt<1>();
        REQUIRE(parser->hasNext() == true);
    }

    SECTION("cursor = 2")
    {
        parser->uInt<1>();
        parser->uInt<1>();
        REQUIRE(parser->hasNext() == true);
    }

    SECTION("cursor = 3")
    {
        parser->uInt<1>();
        parser->uInt<1>();
        parser->uInt<1>();
        REQUIRE(parser->hasNext() == true);
    }

    SECTION("cursor = 4")
    {
        parser->uInt<1>();
        parser->uInt<1>();
        parser->uInt<1>();
        parser->uInt<1>();
        REQUIRE(parser->hasNext() == false);
    }
}