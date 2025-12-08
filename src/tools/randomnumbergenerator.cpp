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
    return std::uniform_int_distribution<int>(0, to)(m_rd);
}

int RandomNumberGenerator::operator()(int from, int to)
{
    return std::uniform_int_distribution<int>(from, to)(m_rd);
}

}  // namespace api
