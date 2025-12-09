#pragma once

#include <QThread>

#include "iworkerhandler.hpp"
#include "api/simulation.hpp"
#include "providers/statistics.hpp"
#include "controllers/simplecontroller.hpp"


namespace workers
{

class SimpleWorkerHandler : public IWorkerHandler
{
    Q_OBJECT

public:
    SimpleWorkerHandler(api::ISimulationDLL* plugin,
                        QObject* parent = nullptr);

    providers::IProvider* controllerProperties() override;
    providers::IProvider* simulationProperties() override;
    providers::IProvider* statistics() override;
    controllers::IController* controller() override;

private:
    controllers::SimpleController m_controller;
    providers::Statistics m_statistics;
    providers::IProvider* m_properties;
    providers::IProvider* m_controllerProperties;
};

}  // namespace workers
