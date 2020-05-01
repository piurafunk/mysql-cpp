#include <catch2/catch.hpp>
#include <MysqlCpp/Support/BufferParser.hpp>

#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>

using MysqlCpp::Support::BufferParser;

TEST_CASE("BufferParser can parse fixed length strings", "[Support/BufferParser - FixedLengthString]")
{
    std::string data = "Hello World! I am here!";
    auto parser = std::make_shared<BufferParser>(std::vector<char>(data.begin(), data.end()));
    unsigned int i = 0;

    SECTION("string<1>")
    {
        std::string result = parser->string(i, 1);

        REQUIRE(result == "H");
    }

    SECTION("string<2>")
    {
        std::string result = parser->string(i, 2);

        REQUIRE(result == "He");
    }

    SECTION("string<3>")
    {
        std::string result = parser->string(i, 3);

        REQUIRE(result == "Hel");
    }

    SECTION("string<4>")
    {
        std::string result = parser->string(i, 4);

        REQUIRE(result == "Hell");
    }

    SECTION("string<5>")
    {
        std::string result = parser->string(i, 5);

        REQUIRE(result == "Hello");
    }

    SECTION("string<1> with offset")
    {
        i += 6;

        std::string result = parser->string(i, 1);

        REQUIRE(result == "W");
    }

    SECTION("string<1> internal")
    {
        std::string result = parser->string<1>();

        REQUIRE(result == "H");
    }

    SECTION("string<2> internal")
    {
        std::string result = parser->string<2>();

        REQUIRE(result == "He");
    }

    SECTION("string<3> internal")
    {
        std::string result = parser->string<3>();

        REQUIRE(result == "Hel");
    }

    SECTION("string<4> internal")
    {
        std::string result = parser->string<4>();

        REQUIRE(result == "Hell");
    }

    SECTION("string<5> internal")
    {
        std::string result = parser->string<5>();

        REQUIRE(result == "Hello");
    }
}

TEST_CASE("BufferParser can parse null terminated strings", "[Support/BufferParser - NullTerminatedString]")
{
    std::vector<char> data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0', ' ', 'I', ' ', 'a', 'm', ' ', 'h', 'e', 'r', 'e', '!', '\0'};
    auto parser = std::make_shared<BufferParser>(data);
    unsigned int i = 0;

    SECTION("string<NULL>")
    {
        std::string result = parser->string(i);

        REQUIRE(result == "Hello World!");
        REQUIRE(i == 13);

        i++;
        result = parser->string(i);

        REQUIRE(result == "I am here!");
        REQUIRE(i == 25);
    }

    SECTION("string<NULL> internal")
    {
        std::string result = parser->string<0>();

        REQUIRE(result == "Hello World!");

        parser->string<1>(); // Skip the space
        result = parser->string<0>();

        REQUIRE(result == "I am here!");
    }
}

TEST_CASE("BufferParser can extract remainder of buffer", "[Support/BufferParser - Remainder]")
{
    std::string data = std::string("Hello World!\0 I am here!", 25);
    auto parser = std::make_shared<BufferParser>(std::vector<char>(data.begin(), data.end()));

    SECTION("string<remainder>")
    {
        std::string result = parser->remainder();

        REQUIRE(result == std::string("Hello World!\0 I am here!", 25));
    }
}

TEST_CASE("std::vector<char> does size corretly", "[std::vector<char>]")
{
    std::vector<char> data{1, 0, 2};

    REQUIRE(data.size() == 3);

    std::stringstream ss;
    for (unsigned int i = 0; i < data.size(); i++)
    {
        ss << std::bitset<2>(data[i]) << ' ';
    }

    std::string s = ss.str();

    REQUIRE(s == "01 00 10 ");
}