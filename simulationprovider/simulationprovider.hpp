#pragma once

#include <QObject>
#include <QStringList>
#include <unordered_map>
#include <set>

#include "api/isimulationdll.hpp"
#include "activesimulationhandler.hpp"


namespace simulationprovider
{

class SimulationProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList simulations READ simulations NOTIFY simulationsChanged)
    Q_PROPERTY(QObjectList properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QObjectList statistics READ statistics NOTIFY statisticsChanged)

signals:
    void errorOccurred(const QString &message);
    void simulationsChanged();
    void propertiesChanged();
    void statisticsChanged();

public:
    explicit SimulationProvider(QObject* parent = nullptr);

    Q_INVOKABLE void select(QString name);

    QStringList simulations() const;
    QObjectList properties() const;
    QObjectList statistics() const;

    void loadSimulations();

private:
    std::unordered_map<QString, api::ISimulationDLL*> m_simulations;
    std::set<QString> m_loadedSimulations;
    std::unique_ptr<ActiveSimulationHandler> m_activeSimulationHandler;
};

}  // namespace simulationprovider
