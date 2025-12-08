#pragma once

#include <QObject>

#include "icontroller.hpp"


namespace controllers
{

class SimpleController : public IController
{
    Q_OBJECT

    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum class State { Ready, Running, Paused, Stopped };
    Q_ENUM(State)

public:
    SimpleController(QObject* parent);

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void restart();

    QUrl uiSource() const override;
    State state() const;

signals:
    void stateChanged();

private:
    State m_state;
};

}  // namespace controllers
