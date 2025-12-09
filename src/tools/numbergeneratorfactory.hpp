#pragma once

#include "api/tools.hpp"


namespace tools
{

enum class NumberGeneratorDistribution
{
    Uniform
};


class NumberGeneratorFactory : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    api::NumberGenerator* create(int seed,
                                 NumberGeneratorDistribution distribiution = NumberGeneratorDistribution::Uniform);
};

}  // namespace tools
