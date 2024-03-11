#include "randomgenerator.hpp"

RandomGenerator::RandomGenerator(int max, int min) noexcept
{
    std::random_device random_device;
    this->_rng = std::default_random_engine(random_device());
    this->_distribution = std::uniform_int_distribution<int>(min, max);
}

int RandomGenerator::generate() noexcept
{
    return this->_distribution(this->_rng);
}
