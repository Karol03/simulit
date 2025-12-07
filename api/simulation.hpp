#pragma once

#include <QtPlugin>
#include <QObject>
#include <QString>

#include "property.hpp"
#include "statistic.hpp"


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
    void progress(const QVariantMap& changes);

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
    virtual api::ISimulation* create() const = 0;
    virtual IProperty* properties() const = 0;
    virtual IStatistic* statistics() const = 0;
};


class SimpleSimulation : public ISimulation
{
    Q_OBJECT

public:
    using ISimulation::ISimulation;

    virtual void setup(QVariantMap properties) = 0;
    virtual void run() = 0;
    virtual void teardown() = 0;

public slots:
    void __setup(QVariantMap properties, QVariantMap statistics)
    {
        stats.reinitialize(std::move(statistics));
    };
    void __start() { emit progress(stats); }
    void __teardown() { teardown(); emit finished(); }

protected:
    utils::BindingMap stats;
};


}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
