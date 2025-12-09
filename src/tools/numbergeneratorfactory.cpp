#include "numbergeneratorfactory.hpp"

#include "determinenumbergenerator.hpp"
#include "randomnumbergenerator.hpp"


namespace tools
{

api::NumberGenerator* NumberGeneratorFactory::create(int seed,
                                                     NumberGeneratorDistribution distribiution)
{
    if (seed == 0)
    {
        return new api::RandomNumberGenerator();
    }
    else
    {
        return new api::DetermineNumberGenerator(seed);
    }
}

}  // namespace tools
