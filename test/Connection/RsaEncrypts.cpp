#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>

#include <string>
#include <iostream>

using namespace std::string_literals;

TEST_CASE("Connector can RSA encrypt and decrypt a password", "[connection-rsa]")
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
        // password is "password"
        std::vector<std::byte> password{std::byte(0x70), std::byte(0x61), std::byte(0x73), std::byte(0x73), std::byte(0x77), std::byte(0x6f), std::byte(0x72), std::byte(0x64)};
        std::string publicKey = "-----BEGIN PUBLIC KEY-----\n"
                                "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEApGgCbvNlZx31DlAafHXv\n"
                                "vDrBpnwRJLNAtyOZWf7bUJ/ViS1FSIDewfgu1AERdZaHiotGCfR310AcZQwPH3ki\n"
                                "2mvchkFNCJlQO464DE3Vmuo3y+UqbS54GhbMY1YHCw18lMTb64zF37m9zR/PssGD\n"
                                "YEb+qY2iirBHh9lZONxDryvoaHA2hs5KgkwTy/gG1w1SqmPQOSLurTR23KfIx2HR\n"
                                "34V9VJ6moLCV3gxDsUqFTrbdtbL5zP8r5EZwULWIgBgD2KlbUC/0g2hszMp1EAce\n"
                                "tGGcnCWcgppiC2g+vj3tNx+kQa/sT35Li7Qj8frf4F9+cGRauyqPv0IcZO4jAeov\n"
                                "uGovQ2ggllSHltpiYJ5Tr7pJy9HV5tu5VRdz666YynHvCmkloN3aABKmbp95Q0JR\n"
                                "BVhgJfraSEr6tLf4hdSDGPe2i4v0FxG4jNsS/PQEpGvAgG1VhM8Y8qD1Y48MIVt9\n"
                                "++DzkvBdHrAP92uye8LabUIcVUntKthscBdep8/NWCE63LKnB5iPX9UtfnBOW2Zv\n"
                                "1th5s9M41co1URcgYcnx3gyn6bbI4lnDGjZnRnwzoGGA9KJQGAuqAE6o8i+dihjh\n"
                                "q74+DioZHNX/aUKDP1yB8HuY6FMtTIkm7Dfc1s2gr3KZSPw/RYlmI4KJgWhGnUAC\n"
                                "pLBqYwq8hCp447jNf0T0mzcCAwEAAQ==\n"
                                "-----END PUBLIC KEY-----";
        std::string privateKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                                 "MIIJQwIBADANBgkqhkiG9w0BAQEFAASCCS0wggkpAgEAAoICAQCkaAJu82VnHfUO\n"
                                 "UBp8de+8OsGmfBEks0C3I5lZ/ttQn9WJLUVIgN7B+C7UARF1loeKi0YJ9HfXQBxl\n"
                                 "DA8feSLaa9yGQU0ImVA7jrgMTdWa6jfL5SptLngaFsxjVgcLDXyUxNvrjMXfub3N\n"
                                 "H8+ywYNgRv6pjaKKsEeH2Vk43EOvK+hocDaGzkqCTBPL+AbXDVKqY9A5Iu6tNHbc\n"
                                 "p8jHYdHfhX1UnqagsJXeDEOxSoVOtt21svnM/yvkRnBQtYiAGAPYqVtQL/SDaGzM\n"
                                 "ynUQBx60YZycJZyCmmILaD6+Pe03H6RBr+xPfkuLtCPx+t/gX35wZFq7Ko+/Qhxk\n"
                                 "7iMB6i+4ai9DaCCWVIeW2mJgnlOvuknL0dXm27lVF3PrrpjKce8KaSWg3doAEqZu\n"
                                 "n3lDQlEFWGAl+tpISvq0t/iF1IMY97aLi/QXEbiM2xL89ASka8CAbVWEzxjyoPVj\n"
                                 "jwwhW3374POS8F0esA/3a7J7wtptQhxVSe0q2GxwF16nz81YITrcsqcHmI9f1S1+\n"
                                 "cE5bZm/W2Hmz0zjVyjVRFyBhyfHeDKfptsjiWcMaNmdGfDOgYYD0olAYC6oATqjy\n"
                                 "L52KGOGrvj4OKhkc1f9pQoM/XIHwe5joUy1MiSbsN9zWzaCvcplI/D9FiWYjgomB\n"
                                 "aEadQAKksGpjCryEKnjjuM1/RPSbNwIDAQABAoICADoTn+BvzL9rfrjKpUCYLNLy\n"
                                 "cpwMMcYkKFVJlw+KjxDhFX665kGhfI9Ky8gKhI9kWIchgeFcYcVBGYa8hs8YbK55\n"
                                 "8dHfWYYTGpufY/a8vBtYZ9N4Jc+h3b1hllv09NH7tqmbDREdjnzk0V6XwTglUfJ9\n"
                                 "wdJFfUi+0tXzeqgybxp/ST8zfJ+MWhfDJi8kc84eH7ifP9wfMe8vnFmAT9XXAVuT\n"
                                 "fV2aGcSj/J0O+g60A3l18cYQ7ltzPJ8MHvrz1b9iPGGaGe6ushKFngGN92JtEXSp\n"
                                 "Eri6NJqSTNY4wglUOOsDG3Q04CaIt8knHpeJhvJs6W5slPyPIW5vvTYORYGUp7xL\n"
                                 "v7Cw/EpKzrAeWWw6uOfq/3ah8Z2gpumVm6Z0jVkN4SEoHGii5t0Ty+bkuUkzLORJ\n"
                                 "qmiZSYnu3kCRZJT26uRGHD0s3U8TD0uvBwSYM/fuNOMtIFkwbvUwcwodXSC0/jLJ\n"
                                 "B2zSqBKyB7H3H1icM8opR1bZiaB49PWFeybWvwhgWEg8Iiq0DEfch3sf1QGE/By8\n"
                                 "VC9jeN49o4nJeJz+WRE8Jk2ixDt+L9tc457QI7lltvQCUdAqemVT9Lpi5Ta0z1LJ\n"
                                 "dHMGRYRiXYuv2MCM7QuGS7D6/z87FTWyBlvbfc0IjzjVMapG5GRounJOG6Nsn179\n"
                                 "gM6hPYWGaaq1O4yyulQpAoIBAQDwy9lVM0Ixyu2lijDgL+3tTMMh/mGJ8oRyJ0pw\n"
                                 "Stt/YkGucfjCb0UjqhKQUqWvkwuZryndITB05OCy4mXRiBOgYzIfeIIxY00UDJMg\n"
                                 "QXMSJWjcPpEoS0qpFtiP+A02YDCQkS/5jGqIzfJ94mSAjLuyYNzQsuEukUZytplP\n"
                                 "CwiTVM4U2Mmg9j0QsHLMExcF5fBvk8BUXUlziePw+H1GloU+egSCAmrsEW1IbX7E\n"
                                 "UUCwgJnY3Nhi3o5k80PdTUy1dwKbQrUkodO0JnIj4b61jiFuBYPlpoPqli0df/gv\n"
                                 "EGYQ9GUqxOrJ/EeyLnf7k79OB9kN/EYEd7jMKE7B3nQh2T7NAoIBAQCuyWsEdq+g\n"
                                 "jaH/DZyPsfym0AKwZFuvCE7X0eua56nu1/GeV95MUfjRJBJc4q1QgS99qd8os7HH\n"
                                 "a6eBQ6hCw4T7UZaEZT7R9CcxOlKz67UMA9a2XvIjmbi9DNX8d1JutRQ0aq58/gD4\n"
                                 "lBRbTdrHEg3+P/bVM5YqjbJ9eJqbXZg448io8o7ML29YXHNnohQ4Zx175YA7gGEm\n"
                                 "73XlTBRtWd6oeicOB7GfCpMOm4mZF+NGWxCsnQI9q9UvsbJDJYFy9rDHsl8sLzyS\n"
                                 "YkftDcEfdxUcmdjWTtyWEXCn9TFPwhitwJuKJj5NAHDiHbiCHkyNzESydIaEByCg\n"
                                 "BLDMW7WrBLoTAoIBAGyZMzJmxMLaJ6XSFkr1stOkHBlXtbvWQX+OUtckG6xurvqy\n"
                                 "8iMWtdLo4Chw7cKqib/NLQ9K6aS+mMx0qMEAIwHh2vgwjF8jzt5s8WnJutEDSDCU\n"
                                 "kzjeBBUQdakwkRzl8uEwabedaWq7n1LA6NzFG93OK6iAFGjBIkmYKrpoWny/8ZWb\n"
                                 "VAwhIddGDPAW5/pEAtT/iO1DiV3vYXsLRsywE/XiZ+gOmRRQo73XjCWUE53G0hvp\n"
                                 "NjoY75V91SyYsTDiNK27PyD/TxBfini+VhlhuZf6KstG9xW0lzJHxbHMLowU/Q91\n"
                                 "8gIYomks5fXzY4MYrP7OuPFI4XJJ94MeBbhCVz0CggEBAJ8hrDhVIlMHf+ypZNgC\n"
                                 "0SbCjSAxTrPN2ZJugZX3wuQ8YLOsBizwGJu4vqsoiWiZJTbAuPbim0IXhlqO0Ztv\n"
                                 "TRxOzL3fO9GE+0W7dt5uA9hbillhmgzFgMdF73C65ib639lVI2WTEClBEEOMX1oM\n"
                                 "2waXD9BN9jLp7Q6FBiXbkaKB6AfBRjcqHLKTAVLh4ozzfIl3ikch+9GqBGlt2c8x\n"
                                 "GiHDjNkeMh29T+PlweHLONWuRUESdZ7WWfLhUo4FkJSteodlo3ajzv7rUdbGRgSy\n"
                                 "s1rlyuBazkQmvq1785hM+5D9od6mMOSEOUH0ky6MDWCwD/91TZ7YqmBlxN1Zxc1d\n"
                                 "MVcCggEBAK4om0TsRszxghugkjTrpuDjdXvw2dxUrUFyoF2dL229hn0MFWMOE3fs\n"
                                 "xM7AsyJUraE7s532PJzb70eDVbAk+6I1hfMmMKc3RpC/VFR0CVea/0VRsF8jAQwb\n"
                                 "4T4TbXR/fqIkGKhKd4XoyoGZXxKL7onBRdb98dMQsrlkqZn0+7o6ZBc1N3KKUDRe\n"
                                 "DIIy14HNqozYAXoManGBP6JWKWhvk1FGxXPtf7obDSdVymIC/51hpgJjxJYyl6Vw\n"
                                 "Dtm1CHnLcRxwBjk0BNx6QtsH7Nd/1gpug24JYrN5I/HlMxEvfJNzV4dl7qaqxxod\n"
                                 "wmURmhR2aqW2JXEKxxC3u7wor0efEsM=\n"
                                 "-----END RSA PRIVATE KEY-----";

        auto encrypted = connection->rsaEncryptPassword(password, publicKey);
        auto decrypted = connection->rsaDecryptPassword(encrypted, privateKey);

        REQUIRE(password == decrypted);
    }
}
