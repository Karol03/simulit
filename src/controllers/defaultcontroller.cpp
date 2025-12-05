#include "defaultcontroller.hpp"


namespace controllers
{

DefaultController::DefaultController(QObject* parent)
    : IController(parent)
    , m_lock{m_mutex}
    , m_waitTime{0}
    , m_state{State::EReady}
{}

void DefaultController::start()
{
    if (m_state == State::EReady)
    {
        m_state = State::ERun;
        m_cv.notify_all();
        emit started();
    }
}

void DefaultController::pause()
{
    if (m_state == State::ERun)
    {
        m_state = State::EPaused;
        m_cv.notify_all();
        emit paused();
    }
}

void DefaultController::stop()
{
    if (m_state == State::ERun || m_state == State::EPaused)
    {
        m_state = State::EStopped;
        m_cv.notify_all();
        emit stopped();
    }
}

bool DefaultController::isReady() const
{
    return m_state == State::EReady;
}

bool DefaultController::isRunning() const
{
    return m_state == State::ERun;
}

bool DefaultController::isStopped() const
{
    return m_state == State::EStopped;
}

bool DefaultController::waitForStart()
{
    m_cv.wait(m_lock, [this](){ return m_state != State::EReady; });
    return m_state == State::ERun;
}

bool DefaultController::wait()
{
    if (m_cv.wait_for(m_lock, m_waitTime, [this](){ return m_state != State::ERun; }))
    {
        m_cv.wait(m_lock, [this](){ return m_state != State::EPaused; });
        if (m_state == State::EStopped)
            return false;
    }
    return true;
}

void DefaultController::setWaitTime(long unsigned timeBetweenStepsMs)
{
    m_waitTime = std::chrono::duration<long unsigned, std::milli>(timeBetweenStepsMs);
}

}  // namespace controllers
