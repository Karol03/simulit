#include "simpleworkerhandler.hpp"

#include "providers/properties.hpp"
#include "providers/statistics.hpp"
#include "providers/workerproperties/simpleworker.hpp"
#include "controllers/simplecontroller.hpp"


namespace workers
{

SimpleWorkerHandler::SimpleWorkerHandler(api::ISimulation* simulation,
                                         api::ISimulationDLL* plugin,
                                         QObject* parent)
    : IWorkerHandler(parent)
    , m_simulation{dynamic_cast<api::SimpleSimulation*>(simulation)}
    , m_ownProperties{ new providers::workerproperties::SimpleWorker(this) }
    , m_simulationProperties{ new providers::Properties(*plugin, this) }
    , m_statistics{ new providers::Statistics(*plugin, this) }
    , m_controller{ new controllers::SimpleController(this) }
{
    m_simulation->setParent(this);
    // create qthread and run it

    /**
     *
    m_controller->setWaitTime(m_config.delayBetweenIterations);
    m_simulation->setup();
    if (m_controller->waitForStart())
    {
        for (auto i = 0; i < m_config.iterations; ++i)
        {
            m_simulation->run();
            m_controller->wait();
        }
    }
    m_simulation->teardown();
*/
    // QObject::connect(m_controller, &controllers::IController::changed, this, &


    // auto rootProperty = m_properties->select("");
    // auto propertyObject = rootProperty->raw();
    // auto property = dynamic_cast<api::IProperty*>(propertyObject);
    // auto parsedProperties = property->parse();
    // auto config = SimpleWorkerConfiguration{
    //     parsedProperties["Przebieg::Liczba przebiegów"].toInt(),
    //     parsedProperties["Przebieg::Ziarno"].toInt(),
    //     parsedProperties["Przebieg::Opóźnienie"].toInt()
    // };
    // update statistics on progress signal
    QObject::connect(m_simulation, &api::ISimulation::progress, m_statistics, &providers::IProvider::change);

    // stop simulation on error
    // controller will display error on screen
    // QObject::connect(m_simulation, &api::ISimulation::error, m_controller, &onError);

    m_simulationThread.start();
}

SimpleWorkerHandler::~SimpleWorkerHandler()
{
    // terminate simulation, exit thread
    if (m_simulationThread.isRunning())
    {
        // m_controller->stop();
        m_simulationThread.quit();
        m_simulationThread.wait();
    }
}


providers::IProvider* SimpleWorkerHandler::ownProperties()
{
    return m_ownProperties;
}

providers::IProvider* SimpleWorkerHandler::simulationProperties()
{
    return m_simulationProperties;
}

providers::IProvider* SimpleWorkerHandler::statistics()
{
    return m_statistics;
}

controllers::IController* SimpleWorkerHandler::controller()
{
    return m_controller;
}

}  // namespace workers
