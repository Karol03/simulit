#include "simulationhandler.hpp"

#include "providers/simulations.hpp"
#include "providers/properties.hpp"
#include "providers/statistics.hpp"
#include "controllers/defaultcontroller.hpp"


SimulationHandler::SimulationHandler(QObject* parent)
    : QObject(parent)
    , m_simulationsProvider{nullptr}
    , m_propertiesProvider{nullptr}
    , m_statisticsProvider{nullptr}
    , m_runtimeController{nullptr}
{}

void SimulationHandler::load()
{
    if (!m_statisticsProvider)
    {
        m_simulationsProvider = new providers::Simulations(this);
        emit simulationsChanged();
    }
}

void SimulationHandler::select(QString name)
{
    auto selectedSimulation = m_simulationsProvider->select(name);
    if (!selectedSimulation)
        return;

    auto simulationPlugin = selectedSimulation->raw();
    if (!simulationPlugin)
        return;

    if (m_loadedSimulationName == name)
    {
        // if providers arleady exists, prevent about
        // reinitialization of the same simulation
        if (m_propertiesProvider && m_statisticsProvider)
            return;
    }

    if (m_runtimeController)
    {
        m_runtimeController->stop();
        m_runtimeController = std::make_shared<controllers::DefaultController>();
    }

    if (m_propertiesProvider)
    {
        m_propertiesProvider->deleteLater();
        m_propertiesProvider = nullptr;
    }

    if (m_statisticsProvider)
    {
        m_statisticsProvider->deleteLater();
        m_statisticsProvider = nullptr;
    }

    if (auto simulationDll = qobject_cast<api::ISimulationDLL*>(simulationPlugin))
    {
        m_propertiesProvider = new providers::Properties(*simulationDll, this);
        m_statisticsProvider = new providers::Statistics(*simulationDll, this);
    }

    std::swap(m_loadedSimulationName, name);

    emit propertiesChanged();
    emit statisticsChanged();
}

QObjectList SimulationHandler::simulations() const
{
    if (m_simulationsProvider)
        return m_simulationsProvider->obtain();
    return {};
}

QObjectList SimulationHandler::properties() const
{
    if (m_propertiesProvider)
        return m_propertiesProvider->obtain();
    return {};
}

QObjectList SimulationHandler::statistics() const
{
    if (m_statisticsProvider)
        return m_statisticsProvider->obtain();
    return {};
}

QObject* SimulationHandler::runtimeController()
{
    if (m_runtimeController)
        return m_runtimeController.get();
    return {};
}
