#pragma once

#include <QObject>
#include <condition_variable>

#include "api/isimulationcontroller.hpp"


namespace controller
{

class SimulationController : public api::ISimulationController
{
public:
    SimulationController(QObject* parent = nullptr);

    bool isReady() const override;
    bool isRunning() const override;
    bool isStopped() const override;

public slots:
    void start() override;
    void pause() override;
    void stop() override;

protected:
    bool waitForStart() override;
    void wait() override;
    void setWaitTime(long unsigned timeBetweenStepsMs) override;

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::unique_lock<std::mutex> m_lock;
    std::chrono::milliseconds m_waitTime;
    api::State m_state;
};

}  // namespace controller
