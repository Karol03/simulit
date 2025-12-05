#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "controllers/icontroller.hpp"
#include "runners/irunner.hpp"


class SimulationHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObjectList simulations READ simulations NOTIFY simulationsChanged)
    Q_PROPERTY(QObjectList runnerProperties READ runnerProperties NOTIFY runnerPropertiesChanged)
    Q_PROPERTY(QObjectList properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QObjectList statistics READ statistics NOTIFY statisticsChanged)
    Q_PROPERTY(QObject* runtimeController READ runtimeController NOTIFY runtimeControllerChanged)

public:
    SimulationHandler(QObject* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void select(QString name);

    QObjectList simulations() const;
    QObjectList runnerProperties() const;
    QObjectList properties() const;
    QObjectList statistics() const;
    QObject* runtimeController();

signals:
    void errorOccurred(const QString &message);
    void simulationsChanged();
    void runnerPropertiesChanged();
    void propertiesChanged();
    void statisticsChanged();
    void runtimeControllerChanged();

private:
    QString m_loadedSimulationName;
    providers::IProvider* m_simulationsProvider;
    providers::IProvider* m_propertiesProvider;
    providers::IProvider* m_statisticsProvider;
    // as shared pointer because is shared with the simulation runner
    std::shared_ptr<controllers::IController> m_runtimeController;
    runners::IRunner* m_simulationRunner;
};
