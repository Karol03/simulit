#pragma once

#include <QtPlugin>
#include <QObject>
#include <QString>

#include "variable.hpp"
#include "tools.hpp"


namespace api
{

class ISimulation : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

signals:
    /**
     * @brief error
     * @param message
     * emit error signal whenever you want to report an error
     * and stop simulation immidiately
     * your code should return immidiately after the signal is emitted
     */
    void error(const QString& message);

    /**
     * @brief progress
     * @param changes
     * progress is automatically updated at the end of the iteration
     * if you need to update it more frequently just call
     * emit progress(stats)
     */
    void progress(const VariableMap::Snapshot& changes);

    /**
     * @brief finished
     * emitted at the end of simulation,
     * do not use it in your code
     */
    void finished();
};


class ISimulationDLL
{
public:
    virtual ~ISimulationDLL() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual ISimulation* create() const = 0;
    virtual Variables properties() const = 0;
    virtual Variables statistics() const = 0;
};


class SimpleSimulation : public ISimulation
{
    Q_OBJECT

public:
    using ISimulation::ISimulation;

    virtual void setup(VariableWatchList properties) = 0;
    virtual void run(NumberGenerator& generator) = 0;
    virtual void teardown() = 0;

public slots:
    void __setup(Variables properties, Variables statistics)
    {
        stats.reinitialize(statistics);
        emit progress(stats);
        setup(VariableMap(properties).watch());
    };
    void __start(NumberGenerator& generator)
    {
        run(generator);
        emit progress(stats);
    }
    void __teardown()
    {
        teardown();
        emit finished();
    }

protected:
    VariableMap stats;
};


}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
