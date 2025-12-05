#include "statistics.hpp"

#include "adapters/statistic.hpp"


namespace providers
{

Statistics::Statistics(api::ISimulationDLL& simulation, QObject* parent)
    : IProvider(parent)
{
    if (!simulation.statistics())
        return;
    m_rootStatistic = simulation.statistics().get();
    traversalMapInsert(m_rootStatistic);
}

QObjectList Statistics::obtain()
{
    auto result = QObjectList{};
    if (m_rootStatistic)
        preorderTraversalSquash(m_rootStatistic, result);
    return result;
}

adapters::IAdapter* Statistics::select(const QString& name)
{
    if (name.isEmpty())
    {
        return new adapters::Statistic(m_rootStatistic, this);
    }
    if (m_statistics.contains(name))
    {
        return new adapters::Statistic(m_statistics[name], this);
    }
    return nullptr;
}

void Statistics::change(const QVariantMap& values)
{
    for (const auto& [name, value] : values.asKeyValueRange())
    {
        if (m_statistics.contains(name) && m_statistics[name])
        {
            m_statistics[name]->set(value);
        }
    }
    emit changed();
};

void Statistics::traversalMapInsert(api::IStatistic* statistic)
{
    if (!statistic->name().isEmpty())
        m_statistics[statistic->name()] = statistic;

    if (auto group = dynamic_cast<api::StatisticGroup*>(statistic))
    {
        for (const auto& child : group->children())
        {
            traversalMapInsert(child.get());
        }
    }
}

void Statistics::preorderTraversalSquash(api::IStatistic* statistic, QObjectList& result)
{
    if (!statistic->name().isEmpty())
        result.append(new adapters::Statistic(statistic, this));
    if (auto group = dynamic_cast<api::StatisticGroup*>(statistic))
    {
        for (const auto& child : group->children())
        {
            preorderTraversalSquash(child.get(), result);
        }
    }
}

}  // namespace providers
