#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstddef>
#include <random>

class RandomGenerator
{
    public:
        RandomGenerator(::size_t max, ::size_t min = 0) noexcept;
        ::size_t generate() noexcept;
    
    private:
        std::default_random_engine _rng;
        std::uniform_int_distribution<::size_t> _distribution;
};

#endif
