#pragma once

#include "api/tools.hpp"
#include <random>


namespace api
{

class RandomNumberGenerator : public NumberGenerator
{
    Q_OBJECT

public:
    int operator()() override;
    int operator()(int to) override;
    int operator()(int from, int to) override;
    double real(double from, double to) override;

private:
    std::random_device m_rd;
};

}  // namespace api
