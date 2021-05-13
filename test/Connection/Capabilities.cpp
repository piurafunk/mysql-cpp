#include <catch2/catch.hpp>
#include <MysqlCpp/Connection.hpp>

SCENARIO("Capabilities are changed", "[connection/capabilities]")
{
    GIVEN("A standard connection with no capabilities")
    {
        auto connection = std::make_shared<MysqlCpp::Connection>("", 0, "", "");
        connection->setCapabilities(0);

        THEN("The capability returns false")
        {
            REQUIRE(connection->capable(MysqlCpp::Connection::Capabilities::CLIENT_PLUGIN_AUTH) == false);
        }

        WHEN("Enabling Connection::Capabilities::CLIENT_PLUGIN_AUTH")
        {
            connection->setCapability(MysqlCpp::Connection::Capabilities::CLIENT_PLUGIN_AUTH, true);

            THEN("The capability returns true")
            {
                REQUIRE(connection->capable(MysqlCpp::Connection::Capabilities::CLIENT_PLUGIN_AUTH) == true);
            }
        }

        WHEN("Re-disabling Connection::Capabilities::CLIENT_PLUGIN_AUTH")
        {
            connection->setCapability(MysqlCpp::Connection::Capabilities::CLIENT_PLUGIN_AUTH, false);

            THEN("The capability returns false")
            {
                REQUIRE(connection->capable(MysqlCpp::Connection::Capabilities::CLIENT_PLUGIN_AUTH) == false);
            }
        }
    }
}
