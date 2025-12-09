#pragma once

#include <QObject>
#include <QQmlEngine>


namespace ControllerState
{

Q_NAMESPACE
QML_ELEMENT

enum State
{
    Ready,
    Running,
    Paused,
    Stopped
};
Q_ENUM_NS(State)

}  // namespace ControllerState
