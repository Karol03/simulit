#pragma once

#include <QPointer>
#include <QObject>

#include "api/simulation.hpp"
#include "iadapter.hpp"


namespace adapters
{

class SimulationRecord : public IAdapter
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)

public:
    SimulationRecord(QObject* simulationPlugin, QObject* parent)
        : IAdapter(parent)
        , m_plugin(simulationPlugin)
    {}
    SimulationRecord(const SimulationRecord&) = delete;
    SimulationRecord& operator=(const SimulationRecord&) = delete;
    SimulationRecord(SimulationRecord&&) = delete;
    SimulationRecord& operator=(SimulationRecord&&) = delete;

    QString name() const
    {
        if (auto simulationModule = qobject_cast<api::ISimulationDLL*>(m_plugin))
        {
            return simulationModule->name();
        }
        return "-- unknown --";
    }

    QObject* raw() override
    {
        return m_plugin;
    }

    const QObject* raw() const override
    {
        return m_plugin;
    }

private:
    QObject* m_plugin;
};

} // namespace adapters
