#pragma once

#include "randomnumbergenerator.hpp"


namespace api
{

int RandomNumberGenerator::operator()()
{
    return m_rd();
}

int RandomNumberGenerator::operator()(int to)
{
    return std::uniform_int_distribution(0, to)(m_rd);
}

int RandomNumberGenerator::operator()(int from, int to)
{
    return std::uniform_int_distribution(from, to)(m_rd);
}

double RandomNumberGenerator::real(double from, double to)
{
    return std::uniform_real_distribution(from, to)(m_rd);
}

}  // namespace api
