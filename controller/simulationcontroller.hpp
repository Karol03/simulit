#pragma once

#include <QObject>
#include <condition_variable>

#include "api/isimulationcontroller.hpp"


namespace api
{

class SimulationController : public ISimulationController
{
    Q_OBJECT

public:
    SimulationController(QObject* parent = nullptr);

public slots:
    void start() override;
    void pause() override;
    void stop() override;

protected:
    bool isReady() const override;
    bool waitForStart() override;
    void wait() override;
    void setWaitTime(long unsigned timeBetweenStepsMs) override;

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::unique_lock<std::mutex> m_lock;
    std::chrono::milliseconds m_waitTime;
    State m_state;
};

}  // namespace api
