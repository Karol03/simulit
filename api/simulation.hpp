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

public slots:
    virtual void setup(QVariantMap properties,
                       QVariantMap statistics) = 0;
    virtual void run() = 0;
    virtual void teardown() = 0;

signals:
    void progress(const QVariantMap& changes);
    void finished();
    void error(const QString& message);
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

}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
