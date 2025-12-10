#include "workerhandlerfactory.hpp"

#include "api/simulation.hpp"
#include "workerhandler.hpp"
#include "controllers/simplecontroller.hpp"
#include "controllers/animatedcontroller.hpp"


namespace workers
{

WorkerHandlerFactory::WorkerHandlerFactory(QObject* parent)
    : QObject(parent)
{
    registerController<api::SimpleSimulation, controllers::SimpleController>();
    registerController<api::AnimatedSimulation, controllers::AnimatedController>();
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
        if (auto controller = factory(simulation, plugin))
        {
            delete simulation;
            return new WorkerHandler(plugin, controller);
        }
    }
    delete simulation;
    return nullptr;
}

}  // namespace workers
