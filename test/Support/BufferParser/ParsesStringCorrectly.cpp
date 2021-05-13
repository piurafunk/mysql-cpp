#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>

using MysqlCpp::Support::BufferParser;

SCENARIO("BufferParser can parse fixed length strings", "[Support/BufferParser - FixedLengthString]")
{
    GIVEN("The sentence \"Hello World! I am here!\"")
    {
        std::string data = "Hello World! I am here!";
        auto parser = std::make_shared<BufferParser>(std::vector<char>(data.begin(), data.end()));
        unsigned int i = 0;

        WHEN("The first byte is fetched")
        {
            std::string result = parser->string(i, 1);

            THEN("The value is 'H'")
            {
                REQUIRE(result == "H");
            }
        }

        WHEN("The first 2 bytes are fetched")
        {
            std::string result = parser->string(i, 2);

            THEN("The value is 'He'")
            {
                REQUIRE(result == "He");
            }
        }

        WHEN("The first 3 bytes are fetched")
        {
            std::string result = parser->string(i, 3);

            THEN("The value is 'Hel'")
            {
                REQUIRE(result == "Hel");
            }
        }

        WHEN("The first 4 bytes are fetched")
        {
            std::string result = parser->string(i, 4);

            THEN("The value is 'Hell'")
            {
                REQUIRE(result == "Hell");
            }
        }

        WHEN("The first 5 bytes are fetched")
        {
            std::string result = parser->string(i, 5);

            THEN("The value is 'Hello'")
            {
                REQUIRE(result == "Hello");
            }
        }

        WHEN("The first 6 are skipped, then 1 byte is pulled")
        {
            i += 6;

            std::string result = parser->string(i, 1);

            THEN("The value is 'W'")
            {
                REQUIRE(result == "W");
            }
        }

        WHEN("The first byte is fetched from the \"internal\" method")
        {
            std::string result = parser->string<1>();

            THEN("The value is 'H'")
            {
                REQUIRE(result == "H");
            }
        }

        WHEN("The first 2 bytes are fetched from the \"internal\" method")
        {
            std::string result = parser->string<2>();

            THEN("The value is 'He'")
            {
                REQUIRE(result == "He");
            }
        }

        WHEN("The first 3 bytes are fetched from the \"internal\" method")
        {
            std::string result = parser->string<3>();

            THEN("The value is 'Hel'")
            {
                REQUIRE(result == "Hel");
            }
        }

        WHEN("The first 4 bytes are fetched from the \"internal\" method")
        {
            std::string result = parser->string<4>();

            THEN("The value is 'Hell'")
            {
                REQUIRE(result == "Hell");
            }
        }

        WHEN("The first 5 bytes are fetched from the \"internal\" method")
        {
            std::string result = parser->string<5>();

            THEN("The value is 'Hello'")
            {
                REQUIRE(result == "Hello");
            }
        }
    }
}

SCENARIO("BufferParser can parse null terminated strings", "[Support/BufferParser - NullTerminatedString]")
{
    GIVEN("A std::vector<char> with multiple null terminated strings within")
    {
        std::vector<char> data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0', ' ', 'I', ' ', 'a', 'm', ' ', 'h', 'e', 'r', 'e', '!', '\0'};
        auto parser = std::make_shared<BufferParser>(data);
        unsigned int i = 0;

        WHEN("Fetching without specifying length")
        {
            std::string result = parser->string(i);

            THEN("The string leading up to the first encountered \\0 is returned")
            {
                REQUIRE(result == "Hello World!");
                REQUIRE(i == 13);
            }

            i++;
            result = parser->string(i);

            THEN("The string leading up to the next encountered \\0 is returned")
            {
                REQUIRE(result == "I am here!");
                REQUIRE(i == 25);
            }
        }

        WHEN("Fetching without specifying length using the \"internal\" function")
        {
            std::string result = parser->string<0>();

            THEN("The string leading up to the first encountered \\0 is returned")
            {
                REQUIRE(result == "Hello World!");
            }

            parser->string<1>(); // Skip the \0
            result = parser->string<0>();

            THEN("The string leading up to the next encountered \\0 is returned")
            {
                REQUIRE(result == "I am here!");
            }
        }
    }
}

SCENARIO("BufferParser can extract remainder of buffer", "[Support/BufferParser - Remainder]")
{
    GIVEN("A string with multiple null terminated strings within")
    {
        std::string data = std::string("Hello World!\0 I am here!", 25);
        auto parser = std::make_shared<BufferParser>(std::vector<char>(data.begin(), data.end()));

        WHEN("Fetching remainder")
        {
            std::string result = parser->remainder();

            THEN("The string equals the remainder")
            {
                REQUIRE(result == std::string("Hello World!\0 I am here!", 25));
            }
        }
    }
}

SCENARIO("std::vector<char> does size correctly", "[std::vector<char>]")
{
    GIVEN("A simple std::vector<char>")
    {
        std::vector<char> data{1, 0, 2};

        THEN("The size is correct")
        {
            REQUIRE(data.size() == 3);
        }

        WHEN("Stringifying as binary in std::stringstream")
        {
            std::stringstream ss;
            for (unsigned int i = 0; i < data.size(); i++)
            {
                ss << std::bitset<2>(data[i]) << ' ';
            }

            std::string s = ss.str();

            THEN("The string is written in human readable binary")
            {
                REQUIRE(s == "01 00 10 ");
            }
        }
    }
}
