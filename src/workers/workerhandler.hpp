#pragma once

#include <QThread>

#include "iworkerhandler.hpp"
#include "api/simulation.hpp"


namespace workers
{

class WorkerHandler : public IWorkerHandler
{
    Q_OBJECT

public:
    WorkerHandler(api::ISimulationDLL* plugin,
                  controllers::IController* ctrl,
                  QObject* parent = nullptr);

    providers::IProvider* controllerProperties() override;
    providers::IProvider* simulationProperties() override;
    providers::IProvider* statistics() override;
    controllers::IController* controller() override;

private:
    controllers::IController* m_controller;
    providers::IProvider* m_statistics;
    providers::IProvider* m_properties;
    providers::IProvider* m_controllerProperties;
};

}  // namespace workers
