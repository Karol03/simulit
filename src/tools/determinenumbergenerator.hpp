#pragma once

#include "api/tools.hpp"
#include <random>


namespace api
{

class DetermineNumberGenerator : public NumberGenerator
{
    Q_OBJECT

public:
    DetermineNumberGenerator(int seed);

    int operator()() override;
    int operator()(int to) override;
    int operator()(int from, int to) override;
    double real(double from, double to) override;

private:
    std::mt19937 m_gen;
};

}  // namespace api
