#pragma once

#include <QPointer>
#include <optional>

#include "iprovider.hpp"
#include "api/simulation.hpp"
#include "api/variable.hpp"


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
    void updateFromMap(const QVariantMap& update) override;
    void updateWatched(const api::VariableMapSnapshot& update);

signals:
    void updated();

private:
    void createAdapters(api::VariableMap statistics);

private:
    QObjectList m_adapters;
    std::optional<api::VariableWatchList> m_watchList;
};

}  // namespace providers
