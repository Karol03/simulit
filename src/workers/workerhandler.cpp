#include "workerhandler.hpp"

#include "providers/properties.hpp"


namespace workers
{

WorkerHandler::WorkerHandler(api::ISimulationDLL* plugin,
                             controllers::IController* ctrl,
                             QObject* parent)
    : IWorkerHandler(parent)
    , m_controller{ctrl}
    , m_statistics{m_controller->statistics()}
    , m_properties{new providers::Properties(plugin->properties(), this)}
    , m_controllerProperties{new providers::Properties(m_controller->properties(), this)}
{
    m_controller->setParent(this);
}

providers::IProvider* WorkerHandler::controllerProperties()
{
    return m_controllerProperties;
}

providers::IProvider* WorkerHandler::simulationProperties()
{
    return m_properties;
}

providers::IProvider* WorkerHandler::statistics()
{
    return m_statistics;
}

controllers::IController* WorkerHandler::controller()
{
    return m_controller;
}

}  // namespace workers
