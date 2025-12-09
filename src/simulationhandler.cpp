#include "simulationhandler.hpp"

#include "providers/simulations.hpp"


SimulationHandler::SimulationHandler(QObject* parent)
    : QObject(parent)
    , m_simulationsProvider{nullptr}
    , m_workers{this}
    , m_selectedWorker{nullptr}
{}

void SimulationHandler::load()
{
    if (!m_simulationsProvider)
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

    auto simulationDll = qobject_cast<api::ISimulationDLL*>(simulationPlugin);
    if (!simulationDll)
        return;
    m_activeSimulationName = simulationDll->name();
    m_activeSimulationDescription = simulationDll->description();

    if (m_workers.exists(name))
    {
        m_selectedWorker = m_workers[name];
    }
    else
    {
        m_selectedWorker = m_workers.create(name, simulationDll);
    }

    emit propertiesChanged();
    emit workerPropertiesChanged();
    emit statisticsChanged();
    emit runtimeControllerChanged();
    emit nameChanged();
    emit descriptionChanged();
}

QObjectList SimulationHandler::simulations() const
{
    if (m_simulationsProvider)
        return m_simulationsProvider->obtain();
    return {};
}

QObjectList SimulationHandler::workerProperties() const
{
    if (m_selectedWorker && m_selectedWorker->controllerProperties())
        return m_selectedWorker->controllerProperties()->obtain();
    return {};
}

QObjectList SimulationHandler::properties() const
{
    if (m_selectedWorker && m_selectedWorker->simulationProperties())
        return m_selectedWorker->simulationProperties()->obtain();
    return {};
}

QObjectList SimulationHandler::statistics() const
{
    if (m_selectedWorker && m_selectedWorker->statistics())
        return m_selectedWorker->statistics()->obtain();
    return {};
}

QObject* SimulationHandler::runtimeController()
{
    if (m_selectedWorker && m_selectedWorker->controller())
        return m_selectedWorker->controller();
    return {};
}

QString SimulationHandler::name() const
{
    return m_activeSimulationName;
}

QString SimulationHandler::description() const
{
    return m_activeSimulationDescription;
}
