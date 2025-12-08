#include "statistics.hpp"

#include "adapters/statistic.hpp"


namespace providers
{

Statistics::Statistics(api::ISimulationDLL& simulation, QObject* parent)
    : IProvider(parent)
{
    if (!simulation.statistics())
        return;
    createAdapters(simulation.statistics());
}

QObjectList Statistics::obtain()
{
    return m_adapters;
}

adapters::IAdapter* Statistics::select(const QString& name)
{
    // select any Statistics do nothing
    return nullptr;
}

void Statistics::createAdapters(api::VariableMap statistics)
{
    m_watchList = statistics.watch();
    m_adapters.reserve(m_watchList->size());
    for (int i = 0; i < statistics.size(); ++i)
    {
        auto variable = statistics.number(i);
        if (!variable->name().isEmpty())
        {
            m_adapters.append(new adapters::Statistic(variable->name(),
                                                      variable->description(),
                                                      *m_watchList,
                                                      i,
                                                      this));
        }
    }
}

void Statistics::updateFromMap(const QVariantMap& update)
{
    m_watchList->update(update);
    emit updated();
}

void Statistics::updateWatched(const api::VariableMapSnapshot& update)
{
    m_watchList->update(update);
    emit updated();
}

}  // namespace providers
