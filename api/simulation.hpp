#pragma once

#include <QtPlugin>
#include <QObject>
#include <QString>
#include <memory>

#include "property.hpp"
#include "statistic.hpp"
#include "isimulationcontroller.hpp"


namespace api
{

class ISimulation : public QObject
{
    Q_OBJECT

public:
    explicit ISimulation(const IPropertyPtr& properties,
                         const IStatisticPtr& statistics,
                         std::shared_ptr<ISimulationController> controller,
                         QObject* parent = nullptr)
        : QObject(parent)
        , props(parse(properties))
        , stats(parse(statistics))
        , controller(controller)
    {}

public slots:
    virtual void run() = 0;

protected:
    void wait() { controller->wait(); }
    void setWaitTime(long unsigned timeBetweenStepsMs) { controller->setWaitTime(timeBetweenStepsMs); }

signals:
    void progress(const QVariantMap& changes);
    void finished();
    void error(const QString& message);

protected:
    const Props props;
    Stats stats;
    std::shared_ptr<ISimulationController> controller;
};

class ISimulationDLL
{
public:
    virtual ~ISimulationDLL() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual std::shared_ptr<ISimulation> simulation(std::shared_ptr<ISimulationController> controller) = 0;

    virtual const IPropertyPtr& properties() { return m_properties; }
    virtual const IStatisticPtr& statistics() { return m_statistics; }

protected:
    IPropertyPtr m_properties;
    IStatisticPtr m_statistics;
};

}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
