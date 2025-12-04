#include "simulationcontroller.hpp"


namespace api
{

SimulationController::SimulationController(QObject* parent)
    : ISimulationController(parent)
    , m_lock{m_mutex}
    , m_waitTime{0}
    , m_state{State::EReady}
{}

void SimulationController::start()
{
    if (m_state == State::EReady)
    {
        m_state = State::ERun;
        m_cv.notify_all();
    }
}

void SimulationController::pause()
{
    if (m_state == State::ERun)
    {
        m_state = State::EPaused;
        m_cv.notify_all();
    }
}

void SimulationController::stop()
{
    if (m_state == State::ERun || m_state == State::EPaused)
    {
        m_state = State::EStopped;
        m_cv.notify_all();
    }
}

bool SimulationController::isReady() const
{
    return m_state == State::EReady;
}

bool SimulationController::waitForStart()
{
    m_cv.wait(m_lock, [this](){ return m_state != State::EReady; });
    return m_state == State::ERun;
}

void SimulationController::wait()
{
    if (m_cv.wait_for(m_lock, m_waitTime, [this](){ return m_state != State::ERun; }))
    {
        m_cv.wait(m_lock, [this](){ return m_state != State::EPaused; });
        if (m_state == State::EStopped)
            return;
    }
}

void SimulationController::setWaitTime(long unsigned timeBetweenStepsMs)
{
    m_waitTime = std::chrono::duration<long unsigned, std::milli>(timeBetweenStepsMs);
}

}  // namespace api
