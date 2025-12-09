#include "workerhandlerfactory.hpp"

#include "api/simulation.hpp"
#include "simpleworkerhandler.hpp"


namespace workers
{

WorkerHandlerFactory::WorkerHandlerFactory(QObject* parent)
    : QObject(parent)
{
    registerHandler<api::SimpleSimulation, SimpleWorkerHandler>();
}

IWorkerHandler* WorkerHandlerFactory::createFrom(api::ISimulationDLL* plugin)
{
    if (!plugin)
        return nullptr;

    auto simulation = plugin->create();
    if (!simulation)
        return nullptr;

    for (const auto& factory : m_factoryMethods)
    {
        if (auto worker = factory(simulation, plugin))
        {
            delete simulation;
            return worker;
        }
    }
    delete simulation;
    return nullptr;
}

}  // namespace workers
