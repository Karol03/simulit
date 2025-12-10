#pragma once
/**
 * This header defines the plugin API for creating simulations loaded by SimuLit.
 * The framework manages statistics, invokes your lifecycle methods, and handles
 * error and progress reporting.
 *
 * To implement a simulation plugin:
 * 1. Derive your plugin class from ISimulationDLL and override:
 *      - name()        : QString           - returns the name of your simulation
 *      - description() : QString           - returns a short description
 *      - create()      : api::ISimulation* - returns a new instance of your simulation class
 *      - properties()  : api::Variables    - returns a pointer to input parameters
 *      - statistics()  : api::Variables    - returns a pointer to output statistics
 *
 *    Properties declared in properties() are passed to your simulation via setup().
 *    Statistics declared in statistics() are stored in the 'stats' map and must be
 *    updated manually in run().
 *
 * 2. Implement your simulation logic by deriving from either:
 *      - SimpleSimulation
 *      - AnimatedSimulation
 *
 * SimpleSimulation:
 *      Receives input parameters in setup() and provides output only through
 *      the 'stats' variable, which you are responsible for updating inside run().
 *
 * AnimatedSimulation:
 *      Extends SimpleSimulation by adding an image rendered in the UI.
 *      Your run() implementation must update both 'stats' and the 'image'
 *      if you want animation frames to be displayed.
 *
 * Required methods in your simulation class:
 *      - setup(VariableWatchList properties) : void
 *          Initializes state using variables defined in ISimulationDLL::properties().
 *          Access values by name, e.g.:
 *              int gates = properties.get<int>("Bramki");
 *
 *      - run(api::NumberGenerator* generator) : void
 *          Executes simulation logic and updates statistics declared in
 *          ISimulationDLL::statistics(). Access statistics by reference, e.g.:
 *              auto& trials = stats.ref<int>("Rozegrane");
 *              ++trials; // updating the reference modifies the stored statistic
 *          This reference semantics is important: modifying the ref updates the
 *          internal statistics used by the framework.
 *          The provided NumberGenerator must be used for randomness—do not use
 *          standard C++ random generators, otherwise UI configuration may not work.
 *          If a fatal error occurs, emit error(message) and return immediately.
 *
 *      - teardown() : void
 *          Releases all resources.

 * Defining properties and statistics:
 *      Variables are declared using api::var(), which can group parameters and nest
 *      them hierarchically. Each var<T> defines a name, description, default value,
 *      and optionally a validator:
 *
 *      props = api::var("Root Group", this,
 *                  api::var<int>("Parameter", "Can be 1-100", 100, [](const int& v) { return 0 < v && v <= 100; } ),
 *                  api::var("Inner Group",
 *                          api::var<double>("Param 1", "Double param", 0.0),
 *                          api::var<bool>("Param 2", "Boolean param", true)));
 *      stats = api::var(this,
 *          api::var<int>("Wins", "Won games", 0),
 *          api::var<int>("Losses", "Lost games", 0));
 *
 *      In setup(), read properties via properties.get<T>("Param 1").
 *      In run(), update statistics via stats.ref<T>("Wins"), which returns a REFERENCE
 *      that must be modified to change the stored values.
 *
 * Optional:
 *      - emit progress(stats) from run() if you need more frequent UI/statistics updates.
 *      - emit error(message) and stop immediately if an unrecoverable error occurs.
 *
 * 3. Do NOT emit _setupFinished, _runFinished, or _teardownFinished.
 *    These are internal framework signals.
 */

#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QImage>

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
};


class ISimulationDLL
{
public:
    virtual ~ISimulationDLL() = default;

    /**
     * @brief name
     * @return name of the simulation
     */
    virtual QString name() const = 0;

    /**
     * @brief description
     * @return simulation description
     */
    virtual QString description() const = 0;

    /**
     * @brief create
     * Creates an instance of your simulation.
     * Usually it is just a new <your_simulation>()
     * @return an instance of your imulation class
     */
    virtual ISimulation* create() const = 0;

    /**
     * @brief properties
     * Properties are variables that determine the course of the simulation.
     * The user can set them before running the simulation,
     * and their current values can be read in the simulation in the setup() method.
     *
     * Variables are created using the api::var function,
     * which defines a group name followed by a list of var<T> objects
     * representing individual simulation parameters.
     * Each var<T> specifies a name, description, default value, and an optional validator,
     * and variables can be further nested into groups to structure parameters hierarchically.
     * eg.
     * props = api::var("Root Group", this,
     *              api::var<int>("Iterations", "Number of simulation runs. Can be 1-100", 100, [](const int& v) { return 0 < v && v <= 100; } ),
     *              api::var("Inner Group",
     *                      api::var<int>("Seed", "Random seed (0 = auto)", 0),
     *                      api::var<bool>("Use RNG", "Enable random number generator", true)));
     *
     * The api::Variables object MUST BE stored inside the plugin,
     * properties() allows access to it.
     * @return variables describing the properties of your simulation
     */
    virtual Variables properties() const = 0;

    /**
     * @brief statistics
     * Statistics are collected during simulation runs.
     * Their values are updated after each run() and on explicit command (using: emit progress(stats)).
     * To update statistics, the user should enter the appropriate values during the run().
     *
     * Statistics use the same variable system as properties (see above)
     *
     * The api::Variables object MUST BE stored inside the plugin,
     * statistics() allows access to it.
     * @return variables describing the statistics of your simulation
     */
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
            emit error(QString("An exception occured during SimpleSimulation setup:\n%1").arg(e.what()));
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
            emit error(QString("An exception occured during SimpleSimulation run:\n%1").arg(e.what()));
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
            emit error(QString("An exception occured during SimpleSimulation teardown:\n%1").arg(e.what()));
        }
    }

signals:
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

protected:
    VariableMap stats;
};


class AnimatedSimulation : public ISimulation
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
            emit _setupFinished(stats, image);
        }
        catch (std::exception& e)
        {
            emit error(QString("An exception occured during AnimatedSimulation setup:\n%1").arg(e.what()));
        }
    };
    void _run(NumberGenerator* generator)
    {
        try
        {
            run(*generator);
            emit _runFinished(stats, image);
        }
        catch (std::exception& e)
        {
            emit error(QString("An exception occured during AnimatedSimulation run:\n%1").arg(e.what()));
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
            emit error(QString("An exception occured during AnimatedSimulation teardown:\n%1").arg(e.what()));
        }
    }

signals:
    // --- Do not use signals below in your code --
    /**
     * @brief _setupFinished
     * emitted at the end of setup stage,
     * do not use it in your code
     */
    void _setupFinished(const VariableMap::Snapshot& clearStats, const QImage& image);

    /**
     * @brief _runFinished
     * emitted at the end of the run,
     * do not use it in your code
     */
    void _runFinished(const VariableMap::Snapshot& changes, const QImage& image);

    /**
     * @brief _teardownFinished
     * emitted at the end of simulation,
     * do not use it in your code
     */
    void _teardownFinished();

protected:
    VariableMap stats;
    QImage image;
};


}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
