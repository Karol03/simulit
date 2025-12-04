#pragma once

#include <QObject>


namespace api
{

enum class State
{
    EReady,
    ERun,
    EPaused,
    EStopped
};

class ISimulation;
class ISimulationController : public QObject
{
    friend class ISimulation;

    Q_OBJECT

public:
    using QObject::QObject;

    ISimulationController(const ISimulationController&) = delete;
    ISimulationController& operator=(const ISimulationController&) = delete;
    ISimulationController(ISimulationController&&) = delete;
    ISimulationController& operator=(ISimulationController&&) = delete;

public slots:
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    virtual bool isReady() const = 0;
    virtual bool waitForStart() = 0;
    virtual void wait() = 0;
    virtual void setWaitTime(long unsigned timeBetweenStepsMs) = 0;
};

}  // namespace api
