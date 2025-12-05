#include "defaultcontroller.hpp"


namespace controllers
{

DefaultController::DefaultController()
    : ISimulationController(nullptr)
    , m_lock{m_mutex}
    , m_waitTime{0}
    , m_state{api::State::EReady}
{}

void DefaultController::start()
{
    if (m_state == api::State::EReady)
    {
        m_state = api::State::ERun;
        m_cv.notify_all();
    }
}

void DefaultController::pause()
{
    if (m_state == api::State::ERun)
    {
        m_state = api::State::EPaused;
        m_cv.notify_all();
    }
}

void DefaultController::stop()
{
    if (m_state == api::State::ERun || m_state == api::State::EPaused)
    {
        m_state = api::State::EStopped;
        m_cv.notify_all();
    }
}

bool DefaultController::isReady() const
{
    return m_state == api::State::EReady;
}

bool DefaultController::isRunning() const
{
    return m_state == api::State::ERun;
}

bool DefaultController::isStopped() const
{
    return m_state == api::State::EStopped;
}

bool DefaultController::waitForStart()
{
    m_cv.wait(m_lock, [this](){ return m_state != api::State::EReady; });
    return m_state == api::State::ERun;
}

void DefaultController::wait()
{
    if (m_cv.wait_for(m_lock, m_waitTime, [this](){ return m_state != api::State::ERun; }))
    {
        m_cv.wait(m_lock, [this](){ return m_state != api::State::EPaused; });
        if (m_state == api::State::EStopped)
            return;
    }
}

void DefaultController::setWaitTime(long unsigned timeBetweenStepsMs)
{
    m_waitTime = std::chrono::duration<long unsigned, std::milli>(timeBetweenStepsMs);
}

}  // namespace controllers
