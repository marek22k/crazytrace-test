#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>

class RandomGenerator
{
    public:
        RandomGenerator(int max, int min = 0) noexcept;
        int generate() noexcept;
    
    private:
        std::default_random_engine _rng;
        std::uniform_int_distribution<int> _distribution;
};

#endif
