#pragma once

#include <QObject>

#include "icontroller.hpp"


namespace controllers
{

class SimpleController : public IController
{
    Q_OBJECT

    enum class State
    {
        EReady,
        ERunning,
        EPaused,
        EStopped
    };

public:
    SimpleController(QObject* parent);

    QUrl uiSource() const override;

private:
    State m_state;
};

}  // namespace controllers
