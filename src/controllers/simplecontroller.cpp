#include "simplecontroller.hpp"


namespace controllers
{

SimpleController::SimpleController(QObject* parent)
    : IController(parent)
    , m_state{State::Ready}
{}

QUrl SimpleController::uiSource() const
{
    return QUrl("qrc:/qt/qml/simulit/gui/controllers/SimpleController.qml");
}

SimpleController::State SimpleController::state() const
{
    return m_state;
}

void SimpleController::start()
{
    if (m_state == State::Ready || m_state == State::Paused)
    {
        m_state = State::Running;
        emit stateChanged();
    }
}

void SimpleController::pause()
{
    if (m_state == State::Running)
    {
        m_state = State::Paused;
        emit stateChanged();
    }
}

void SimpleController::stop()
{
    if (m_state == State::Running || m_state == State::Paused)
    {
        m_state = State::Stopped;
        emit stateChanged();
    }
}

void SimpleController::restart()
{
    if (m_state == State::Running || m_state == State::Paused || m_state == State::Stopped)
    {
        m_state = State::Ready;
        emit stateChanged();
    }
}



// void SimpleController::onNotReady()
// {
//     if (m_state == State::EReady)
//     {
//         m_state = State::ERun;
//         m_cv.notify_all();
//         emit started();
//     }
// }

// void SimpleController::onReady()
// {
//     if (m_state == State::ERun)
//     {
//         m_state = State::EPaused;
//         m_cv.notify_all();
//         emit paused();
//     }
// }

// void SimpleController::stop()
// {
//     if (m_state == State::ERun || m_state == State::EPaused)
//     {
//         m_state = State::EStopped;
//         m_cv.notify_all();
//         emit stopped();
//     }
// }

// bool SimpleController::isReady() const
// {
//     return m_state == State::EReady;
// }

// bool SimpleController::isRunning() const
// {
//     return m_state == State::ERun;
// }

// bool SimpleController::isStopped() const
// {
//     return m_state == State::EStopped;
// }

// bool SimpleController::waitForStart()
// {
//     m_cv.wait(m_lock, [this](){ return m_state != State::EReady; });
//     return m_state == State::ERun;
// }

// bool SimpleController::wait()
// {
//     if (m_cv.wait_for(m_lock, m_waitTime, [this](){ return m_state != State::ERun; }))
//     {
//         m_cv.wait(m_lock, [this](){ return m_state != State::EPaused; });
//         if (m_state == State::EStopped)
//             return false;
//     }
//     return true;
// }

// void SimpleController::setWaitTime(long unsigned timeBetweenStepsMs)
// {
//     m_waitTime = std::chrono::duration<long unsigned, std::milli>(timeBetweenStepsMs);
// }

}  // namespace controllers
