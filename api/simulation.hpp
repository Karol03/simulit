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
     * statistics are automatically update
     * at the end of the run function
     * if you need to update it more frequently just call
     * emit progress(stats)
     */
    void progress(const VariableMap::Snapshot& changes);

    // --- Do not use signals below in your code --
    /**
     * @brief _setupFinished
     * emitted at the end of setup stage,
     * do not use it in your code
     */
    void _setupFinished(const VariableMap::Snapshot& clearStats);

    /**
     * @brief _runFinished
     * emitted at the end of the run,
     * do not use it in your code
     */
    void _runFinished(const VariableMap::Snapshot& changes);

    /**
     * @brief _teardownFinished
     * emitted at the end of simulation,
     * do not use it in your code
     */
    void _teardownFinished();
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
    void _setup(Variables properties, Variables statistics)
    {
        try
        {
            stats.reinitialize(statistics);
            stats.reset();
            setup(VariableMap(properties).watch());
            emit _setupFinished(stats);
        }
        catch (std::exception& e)
        {
            emit error(QString("An exception occured during the simulation setup:\n%1").arg(e.what()));
        }
    };
    void _run(NumberGenerator* generator)
    {
        try
        {
            run(*generator);
            emit _runFinished(stats);
        }
        catch (std::exception& e)
        {
            emit error(QString("An exception occured during the simulation run:\n%1").arg(e.what()));
        }
    }
    void _teardown()
    {
        try
        {
            teardown();
            emit _teardownFinished();
        }
        catch (std::exception& e)
        {
            emit error(QString("An exception occured during the simulation teardown:\n%1").arg(e.what()));
        }
    }

signals:

protected:
    VariableMap stats;
};


}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
