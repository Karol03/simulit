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

}  // namespace controllers
