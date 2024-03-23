#include <gtest/gtest.h>
#include <boost/log/trivial.hpp>
#include "loglevel.hpp"

TEST(LogLevelTest, Set)
{
    EXPECT_EQ(LogLevel("trace").get_log_level(), boost::log::trivial::trace);
    EXPECT_EQ(LogLevel("debug").get_log_level(), boost::log::trivial::debug);
    EXPECT_EQ(LogLevel("info").get_log_level(), boost::log::trivial::info);
    EXPECT_EQ(LogLevel("warning").get_log_level(),
              boost::log::trivial::warning);
    EXPECT_EQ(LogLevel("error").get_log_level(), boost::log::trivial::error);
    EXPECT_EQ(LogLevel("fatal").get_log_level(), boost::log::trivial::fatal);

    try
    {
        const LogLevel test("test");
        FAIL();
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()), "Unknown log level");
    }

    EXPECT_EQ(LogLevel(boost::log::trivial::trace).get_log_level(),
              boost::log::trivial::trace);
    EXPECT_EQ(LogLevel(boost::log::trivial::debug).get_log_level(),
              boost::log::trivial::debug);
    EXPECT_EQ(LogLevel(boost::log::trivial::info).get_log_level(),
              boost::log::trivial::info);
    EXPECT_EQ(LogLevel(boost::log::trivial::warning).get_log_level(),
              boost::log::trivial::warning);
    EXPECT_EQ(LogLevel(boost::log::trivial::error).get_log_level(),
              boost::log::trivial::error);
    EXPECT_EQ(LogLevel(boost::log::trivial::fatal).get_log_level(),
              boost::log::trivial::fatal);
}
