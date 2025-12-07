#pragma once

#include <QThread>

#include "iworkerhandler.hpp"
#include "api/simulation.hpp"


namespace workers
{

class SimpleWorkerHandler : public IWorkerHandler
{
    Q_OBJECT

public:
    SimpleWorkerHandler(api::ISimulation* simulation,
                        api::ISimulationDLL* plugin,
                        QObject* parent = nullptr);
    ~SimpleWorkerHandler();

    providers::IProvider* ownProperties() override;
    providers::IProvider* simulationProperties() override;
    providers::IProvider* statistics() override;
    controllers::IController* controller() override;

private:
    api::ISimulation* m_simulation;
    providers::IProvider* m_ownProperties;
    providers::IProvider* m_simulationProperties;
    providers::IProvider* m_statistics;
    controllers::IController* m_controller;
    QThread m_simulationThread;
};

}  // namespace workers
