#pragma once

#include <vector>
#include <functional>

#include "api/simulation.hpp"
#include "iworkerhandler.hpp"
#include "controllers/icontroller.hpp"


namespace workers
{


class WorkerHandlerFactory : public QObject
{
    Q_OBJECT

    using FactoryMethod = std::function<controllers::IController*(api::ISimulation*,
                                                                  api::ISimulationDLL*)>;

public:
    WorkerHandlerFactory(QObject* parent);

    IWorkerHandler* createFrom(api::ISimulationDLL* plugin);

private:
    template<typename SimulationT, typename ControllerT>
    void registerController() {
        m_factoryMethods.emplace_back(
            [](api::ISimulation* simulation, api::ISimulationDLL* plugin) -> ControllerT* {
                return dynamic_cast<SimulationT*>(simulation) ? new ControllerT(plugin) : nullptr;
            });
    }

private:
    std::vector<FactoryMethod> m_factoryMethods;
};

}  // namespace workers
