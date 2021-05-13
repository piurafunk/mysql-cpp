#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <memory>

using MysqlCpp::Support::BufferParser;

SCENARIO("BufferParser knows when it has next", "[Support/BufferParser - hasNext]")
{
    GIVEN("A simple integer buffer containing 4 bytes")
    {
        auto parser = std::make_shared<BufferParser>(std::vector<char>({0b1, 0b10, 0b11, 0b100}));

        WHEN("The cursor is at 0")
        {
            THEN("The buffer returns true for hasNext()")
            {
                REQUIRE(parser->hasNext() == true);
            }
        }

        WHEN("The cursor is at 1")
        {
            parser->uInt<1>();

            THEN("The buffer returns true for hasNext()")
            {
                REQUIRE(parser->hasNext() == true);
            }
        }

        WHEN("The cursor is at 2")
        {
            parser->uInt<2>();

            THEN("The buffer returns true for hasNext()")
            {
                REQUIRE(parser->hasNext() == true);
            }
        }

        WHEN("The cursor is at 3")
        {
            parser->uInt<3>();

            THEN("The buffer returns true for hasNext()")
            {
                REQUIRE(parser->hasNext() == true);
            }
        }

        WHEN("The cursor is at 4")
        {
            parser->uInt<4>();

            THEN("The buffer returns false for hasNext()")
            {
                REQUIRE(parser->hasNext() == false);
            }
        }
    }
}
