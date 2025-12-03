#pragma once

#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QQmlEngine>

#include "property.hpp"
#include "statistic.hpp"


class QQuickItem;

namespace api
{

class ISimulationDLL
{
public:
    virtual ~ISimulationDLL() = default;

    /**
     * @brief name
     * @return short name of the simulation, should be
     */
    virtual QString name() const = 0;
    virtual QString description() const = 0;

    virtual const IPropertyPtr& properties() = 0;
    virtual const IStatisticPtr& statistics() = 0;

    virtual QQuickItem* view(QQmlEngine* engine, QObject* parent = nullptr) = 0;
};

}  // namespace api


#define ISimulationDLL_iid "com.simulit.ISimulationDLL"
Q_DECLARE_INTERFACE(api::ISimulationDLL, ISimulationDLL_iid)
