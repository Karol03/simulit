#pragma once

#include <QObject>


namespace controllers
{

enum class State
{
    EReady,
    ERun,
    EPaused,
    EStopped
};

class IController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isReady READ isReady CONSTANT)
    Q_PROPERTY(bool isRunning READ isRunning CONSTANT)
    Q_PROPERTY(bool isStopped READ isStopped CONSTANT)

public:
    using QObject::QObject;

    IController(const IController&) = delete;
    IController& operator=(const IController&) = delete;
    IController(IController&&) = delete;
    IController& operator=(IController&&) = delete;

    virtual bool isReady() const = 0;
    virtual bool isRunning() const = 0;
    virtual bool isStopped() const = 0;

    virtual bool waitForStart() = 0;
    virtual bool wait() = 0;
    virtual void setWaitTime(long unsigned timeBetweenStepsMs) = 0;

signals:
    void started();
    void paused();
    void stopped();

public slots:
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

}  // namespace controllers
