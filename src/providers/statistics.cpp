#include "statistics.hpp"

#include "adapters/statistic.hpp"


namespace providers
{

Statistics::Statistics(api::ISimulationDLL& simulation, QObject* parent)
    : IProvider(parent)
{
    if (!simulation.statistics())
        return;
    m_rootStatistic = simulation.statistics();
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

void Statistics::traversalMapInsert(api::common::IHierarchicalNamedVariable* statistic)
{
    if (!statistic->name().isEmpty())
        m_statistics[statistic->name()] = dynamic_cast<api::IStatistic*>(statistic);

    if (auto group = dynamic_cast<api::StatisticGroup*>(statistic))
    {
        for (const auto& child : group->inner())
        {
            traversalMapInsert(child);
        }
    }
}

void Statistics::preorderTraversalSquash(api::common::IHierarchicalNamedVariable* statistic,
                                         QObjectList& result)
{
    if (!statistic->name().isEmpty())
        result.append(new adapters::Statistic(dynamic_cast<api::IStatistic*>(statistic), this));
    if (auto group = dynamic_cast<api::StatisticGroup*>(statistic))
    {
        for (const auto& child : group->inner())
        {
            preorderTraversalSquash(child, result);
        }
    }
}

}  // namespace providers
