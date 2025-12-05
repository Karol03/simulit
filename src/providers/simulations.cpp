#include "simulations.hpp"

#include "loaders/dllloader.hpp"
#include "adapters/simulationrecord.hpp"

#include "api/simulation.hpp"


namespace providers
{

Simulations::Simulations(QObject* parent)
    : IProvider(parent)
{
    Q_ASSERT(parent);

    auto loader = loaders::DllLoader(parent);
    auto simulationsDirectory = QString("simulations");
    auto plugins = loader.load(simulationsDirectory);
    for (const auto& plugin : plugins)
    {
        auto simulation = qobject_cast<api::ISimulationDLL*>(plugin);
        m_simulations.insert(simulation->name(), plugin);
    }
}

QObjectList Simulations::obtain()
{
    auto result = QObjectList{};
    for (auto& simulation : m_simulations)
    {
        result.push_back(new adapters::SimulationRecord(simulation, this));
    }
    return result;
}

adapters::IAdapter* Simulations::select(const QString& name)
{
    if (m_simulations.contains(name))
    {
        return new adapters::SimulationRecord(m_simulations[name], this);
    }
    return nullptr;
}

}  // namespace providers
