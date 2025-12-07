#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "workers/pool.hpp"


class SimulationHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObjectList simulations READ simulations NOTIFY simulationsChanged)
    Q_PROPERTY(QObjectList workerProperties READ workerProperties NOTIFY workerPropertiesChanged)
    Q_PROPERTY(QObjectList properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QObjectList statistics READ statistics NOTIFY statisticsChanged)
    Q_PROPERTY(QObject* runtimeController READ runtimeController NOTIFY runtimeControllerChanged)

public:
    SimulationHandler(QObject* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void select(QString name);

    QObjectList simulations() const;
    QObjectList workerProperties() const;
    QObjectList properties() const;
    QObjectList statistics() const;
    QObject* runtimeController();

signals:
    void errorOccurred(const QString &message);
    void simulationsChanged();
    void workerPropertiesChanged();
    void propertiesChanged();
    void statisticsChanged();
    void runtimeControllerChanged();

private:
    QString m_loadedSimulationName;
    providers::IProvider* m_simulationsProvider;
    workers::Pool m_workers;
    workers::IWorkerHandler* m_selectedWorker;
};
