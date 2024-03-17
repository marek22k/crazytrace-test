#include "randomgenerator.hpp"

RandomGenerator::RandomGenerator(std::size_t max, std::size_t min) noexcept :
    _rng(std::random_device()()),
    _distribution(min, max)
{
}

std::size_t RandomGenerator::generate() noexcept
{
    return this->_distribution(this->_rng);
}
