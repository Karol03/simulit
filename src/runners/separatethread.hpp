#pragma once

#include <QThread>
#include "irunner.hpp"
#include "iworker.hpp"


namespace runners
{

class SeparateThread : public IRunner
{
    Q_OBJECT

public:
    SeparateThread(QObject* parent);
    ~SeparateThread();

    void setup(api::ISimulation* simulation,
               std::shared_ptr<controllers::IController> controller,
               providers::IProvider* statisticsToUpdate) override;
    providers::IProvider* properties() override;

private:
    void simulationLoop();

private:
    providers::IProvider* m_properties;
    std::shared_ptr<controllers::IController> m_controller;
    QPointer<providers::IProvider> m_statisticsToUpdate;
    IWorker* m_worker;
    QThread* m_simulationThread;
};

}  // namespace runners
