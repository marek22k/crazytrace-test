#include "randomgenerator.hpp"

RandomGenerator::RandomGenerator(::size_t max, ::size_t min) noexcept :
    _rng(std::random_device()()), _distribution(min, max)
{}

::size_t RandomGenerator::generate() noexcept
{
    return this->_distribution(this->_rng);
}
