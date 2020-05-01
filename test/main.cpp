#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

struct MyListener : Catch::TestEventListenerBase
{

    using TestEventListenerBase::TestEventListenerBase; // inherit constructor

    // Tests starting and ending
    void testCaseStarting(Catch::TestCaseInfo const &testInfo) override
    {
        // Perform some setup before a test case is run
    }

    void testCaseEnded(Catch::TestCaseStats const &testCaseStats) override
    {
        // Tear-down after a test case is run
    }

    // Sections starting and ending
    virtual void sectionStarting(Catch::SectionInfo const &sectionInfo)
    {
        // Perform some setup before a test section is run
    }

    virtual void sectionEnded(Catch::SectionStats const &sectionStats)
    {
        // Tear-down after a test section is run
    }
};

CATCH_REGISTER_LISTENER(MyListener)
