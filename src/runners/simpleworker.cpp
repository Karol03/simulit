#include "simpleworker.hpp"


namespace runners
{

SimpleWorker::SimpleWorker(api::ISimulation* simulation,
                           std::shared_ptr<controllers::IController> controller,
                           SimpleWorkerConfiguration config,
                           QObject* parent)
    : IWorker(parent)
    , m_simulation(simulation)
    , m_controller(controller)
    , m_config(config)
{
    Q_ASSERT(m_simulation);
    Q_ASSERT(m_controller);
    m_simulation->setParent(this);
}

void SimpleWorker::start()
{
    m_controller->setWaitTime(m_config.delayBetweenIterations);
    m_simulation->setup();
    if (m_controller->waitForStart())
    {
        for (auto i = 0; i < m_config.iterations; ++i)
        {
            m_simulation->run();
            m_controller->wait();
        }
    }
    m_simulation->teardown();
}

}  // namespace runners
