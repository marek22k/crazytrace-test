#include <gtest/gtest.h>
#include "randomgenerator.hpp"

TEST(RandomGeneration, Generate)
{
    RandomGenerator gen(10);
    EXPECT_LE(gen.generate(), 10);
}
