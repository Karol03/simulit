#pragma once

#include <QObject>
#include <QStringList>
#include <unordered_map>
#include <set>

#include "api/simulation.hpp"
#include "activesimulationhandler.hpp"


namespace simulationprovider
{

class SimulationProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList simulations READ simulations NOTIFY simulationsChanged)
    Q_PROPERTY(QObjectList properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QObjectList statistics READ statistics NOTIFY statisticsChanged)
    Q_PROPERTY(QObject* controller READ controller NOTIFY controllerChanged)

public:
    explicit SimulationProvider(QObject* parent = nullptr);

    Q_INVOKABLE void select(QString name);

    QStringList simulations() const;
    QObjectList properties() const;
    QObjectList statistics() const;
    QObject* controller();

    void loadSimulations();

signals:
    void errorOccurred(const QString &message);
    void simulationsChanged();
    void propertiesChanged();
    void statisticsChanged();
    void controllerChanged();

private:
    std::unordered_map<QString, api::ISimulationDLL*> m_simulations;
    std::set<QString> m_loadedSimulations;
    std::unique_ptr<ActiveSimulationHandler> m_activeSimulationHandler;
    std::shared_ptr<api::ISimulationController> m_simulationController;
};

}  // namespace simulationprovider
