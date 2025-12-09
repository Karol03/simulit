#pragma once

#include <vector>
#include <type_traits>
#include <functional>

#include "api/simulation.hpp"
#include "iworkerhandler.hpp"


namespace workers
{


class WorkerHandlerFactory : public QObject
{
    Q_OBJECT

    using FactoryMethod = std::function<IWorkerHandler*(api::ISimulation*,
                                                        api::ISimulationDLL*)>;

public:
    WorkerHandlerFactory(QObject* parent);

    IWorkerHandler* createFrom(api::ISimulationDLL* plugin);

private:
    template<typename SimulationT, typename HandlerT>
    void registerHandler() {
        m_factoryMethods.emplace_back(
            [](api::ISimulation* simulation, api::ISimulationDLL* plugin) -> HandlerT* {
                return dynamic_cast<SimulationT*>(simulation) ? new HandlerT(plugin) : nullptr;
            });
    }

private:
    std::vector<FactoryMethod> m_factoryMethods;
};

}  // namespace workers
