#include "simpleworkerhandler.hpp"

#include "providers/properties.hpp"
#include "providers/statistics.hpp"
#include "controllers/simplecontroller.hpp"


namespace workers
{

SimpleWorkerHandler::SimpleWorkerHandler(api::ISimulationDLL* plugin,
                                         QObject* parent)
    : IWorkerHandler(parent)
    , m_controller{plugin, m_statistics}
    , m_statistics{plugin->statistics()}
    , m_properties{new providers::Properties(plugin->properties(), this)}
    , m_controllerProperties{new providers::Properties(m_controller.properties(), this)}
{}

providers::IProvider* SimpleWorkerHandler::controllerProperties()
{
    return m_controllerProperties;
}

providers::IProvider* SimpleWorkerHandler::simulationProperties()
{
    return m_properties;
}

providers::IProvider* SimpleWorkerHandler::statistics()
{
    return &m_statistics;
}

controllers::IController* SimpleWorkerHandler::controller()
{
    return &m_controller;
}

}  // namespace workers
