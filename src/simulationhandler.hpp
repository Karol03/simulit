#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "api/isimulationcontroller.hpp"


class SimulationHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObjectList simulations READ simulations NOTIFY simulationsChanged)
    Q_PROPERTY(QObjectList cProperties READ cProperties NOTIFY cPropertiesChanged)
    Q_PROPERTY(QObjectList properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QObjectList statistics READ statistics NOTIFY statisticsChanged)
    Q_PROPERTY(QObject* runtimeController READ runtimeController NOTIFY runtimeControllerChanged)

public:
    SimulationHandler(QObject* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void select(QString name);

    QObjectList simulations() const;
    QObjectList cProperties() const;
    QObjectList properties() const;
    QObjectList statistics() const;
    QObject* runtimeController();

signals:
    void errorOccurred(const QString &message);
    void simulationsChanged();
    void cPropertiesChanged();
    void propertiesChanged();
    void statisticsChanged();
    void runtimeControllerChanged();

private:
    QString m_loadedSimulationName;
    providers::IProvider* m_simulationsProvider;
    providers::IProvider* m_cPropertiesProvider;
    providers::IProvider* m_propertiesProvider;
    providers::IProvider* m_statisticsProvider;
    // as shared pointer because is shared with the simulation worker
    std::shared_ptr<api::ISimulationController> m_runtimeController;
};
