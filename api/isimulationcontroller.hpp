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
    Q_PROPERTY(bool isReady READ isReady CONSTANT)
    Q_PROPERTY(bool isRunning READ isRunning CONSTANT)
    Q_PROPERTY(bool isStopped READ isStopped CONSTANT)

public:
    using QObject::QObject;

    ISimulationController(const ISimulationController&) = delete;
    ISimulationController& operator=(const ISimulationController&) = delete;
    ISimulationController(ISimulationController&&) = delete;
    ISimulationController& operator=(ISimulationController&&) = delete;


    virtual bool isReady() const = 0;
    virtual bool isRunning() const = 0;
    virtual bool isStopped() const = 0;

public slots:
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    virtual bool waitForStart() = 0;
    virtual void wait() = 0;
    virtual void setWaitTime(long unsigned timeBetweenStepsMs) = 0;
};

}  // namespace api
