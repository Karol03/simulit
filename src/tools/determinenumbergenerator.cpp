#pragma once

#include "determinenumbergenerator.hpp"


namespace api
{

DetermineNumberGenerator::DetermineNumberGenerator(int seed)
    : m_gen(seed)
{}

int DetermineNumberGenerator::operator()()
{
    return m_gen();
}

int DetermineNumberGenerator::operator()(int to)
{
    return std::uniform_int_distribution<int>(0, to)(m_gen);
}

int DetermineNumberGenerator::operator()(int from, int to)
{
    return std::uniform_int_distribution<int>(from, to)(m_gen);
}

}  // namespace api
