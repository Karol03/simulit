#pragma once

#include <QPointer>

#include "iprovider.hpp"
#include "api/simulation.hpp"
#include "api/statistic.hpp"


namespace providers
{

class Statistics : public IProvider
{
    Q_OBJECT

public:
    Statistics(api::ISimulationDLL& simulation, QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void change(const QVariantMap&) override;

private:
    void traversalMapInsert(api::IStatistic* property);
    void preorderTraversalSquash(api::IStatistic* property, QObjectList& result);

private:
    QPointer<api::IStatistic> m_rootStatistic;
    QMap<QString, QPointer<api::IStatistic>> m_statistics;
};

}  // namespace providers
