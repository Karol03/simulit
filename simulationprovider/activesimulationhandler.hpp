#pragma once

#include <QObject>
#include <QThread>

#include "api/simulation.hpp"
#include "api/isimulationcontroller.hpp"
#include "gui/properties/propertyadapter.hpp"
#include "gui/statistics/statisticadapter.hpp"


namespace simulationprovider
{

class ActiveSimulationHandler : public QObject
{
    Q_OBJECT

public:
    ActiveSimulationHandler(QString name, api::ISimulationDLL* simulationDll, QObject* parent = nullptr);
    ~ActiveSimulationHandler();

    const QString& name() const;
    const std::unique_ptr<gui::properties::PropertyAdapter>& properties() const;
    const std::unique_ptr<gui::statistics::StatisticAdapter>& statistics() const;
    const std::shared_ptr<api::ISimulationController>& controller() const;

private:
    void loadProperties();
    void loadStatistis();
    void createPropertyAdapter(QObject* property, gui::properties::PropertyAdapter* parent);
    void prepareSimulation();

private:
    const QString m_name;
    api::ISimulationDLL* const m_simulationDll;
    std::unique_ptr<gui::properties::PropertyAdapter> m_propertyAdapters;
    std::unique_ptr<gui::statistics::StatisticAdapter> m_statisticsAdapters;
    std::shared_ptr<api::ISimulationController> m_simulationController;
    std::shared_ptr<api::ISimulation> m_simulation;
    std::unique_ptr<QThread> m_simulationThread;
};

}  // namespace simulationprovider
