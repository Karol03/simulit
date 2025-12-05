#pragma once

#include "iworker.hpp"

#include "controllers/icontroller.hpp"
#include "api/simulation.hpp"


namespace runners
{

struct SimpleWorkerConfiguration
{
    int iterations;
    int delayBetweenIterations;
    int seed;
};

class SimpleWorker : public IWorker
{
    Q_OBJECT

public:
    SimpleWorker(api::ISimulation* simulation,
                 std::shared_ptr<controllers::IController> controller,
                 SimpleWorkerConfiguration config,
                 QObject* parent);

public:
    void start() override;

private:
    api::ISimulation* m_simulation;
    std::shared_ptr<controllers::IController> m_controller;
    const SimpleWorkerConfiguration m_config;
};

}  // namespace runners
