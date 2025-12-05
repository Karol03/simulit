#include "activesimulationhandler.hpp"

#include "controller/simulationcontroller.hpp"


namespace simulationprovider
{

ActiveSimulationHandler::ActiveSimulationHandler(QString name,
                                                 api::ISimulationDLL* simulationDll,
                                                 QObject* parent)
    : QObject{parent}
    , m_name{std::move(name)}
    , m_simulationDll{std::move(simulationDll)}
{
    loadProperties();
    loadStatistis();
    prepareSimulation();
}

ActiveSimulationHandler::~ActiveSimulationHandler()
{
    if (m_simulationController)
        m_simulationController->stop();
    if (m_simulationThread)
    {
        m_simulationThread->quit();
        m_simulationThread->wait();
        m_simulation->deleteLater();
        m_simulationThread->deleteLater();
        m_simulationThread = nullptr;
    }
}

const QString& ActiveSimulationHandler::name() const
{
    return m_name;
}

void ActiveSimulationHandler::loadProperties()
{
    if (auto& propertyApi = m_simulationDll->properties())
    {
        m_propertyAdapters = std::make_unique<gui::properties::PropertyAdapter>(propertyApi.get());
        for (auto* child : propertyApi->children())
        {
            createPropertyAdapter(child, m_propertyAdapters.get());
        }
    }
}

void ActiveSimulationHandler::createPropertyAdapter(QObject* propertyOobject,
                                                    gui::properties::PropertyAdapter* parent)
{
    if (auto* property = dynamic_cast<api::IProperty*>(propertyOobject))
    {
        auto adapter = new gui::properties::PropertyAdapter(property, parent);
        for (auto* child : property->children())
        {
            createPropertyAdapter(child, adapter);
        }
    }
}

void ActiveSimulationHandler::loadStatistis()
{
    if (auto& statisticApi = m_simulationDll->statistics())
    {
        m_statisticsAdapters = std::make_unique<gui::statistics::StatisticAdapter>(statisticApi.get());
    }
}

void ActiveSimulationHandler::prepareSimulation()
{
    m_simulation = m_simulationDll->simulation(m_simulationController);
    m_simulationThread = std::make_unique<QThread>();

    m_simulation->moveToThread(m_simulationThread.get());
    connect(m_simulationThread.get(), &QThread::started, m_simulation.get(), &api::ISimulation::run);
    connect(m_simulation.get(), &api::ISimulation::finished, this, [this]() {
        m_simulationThread->quit();
        m_simulationThread->wait();
        m_simulation->deleteLater();
        m_simulationThread->deleteLater();
        m_simulationThread = nullptr;
    });
}

const std::unique_ptr<gui::properties::PropertyAdapter>& ActiveSimulationHandler::properties() const
{
    return m_propertyAdapters;
}

const std::unique_ptr<gui::statistics::StatisticAdapter>& ActiveSimulationHandler::statistics() const
{
    return m_statisticsAdapters;
}

const std::shared_ptr<api::ISimulationController>& ActiveSimulationHandler::controller() const
{
    return m_simulationController;
}

}  // namespace simulationprovider
