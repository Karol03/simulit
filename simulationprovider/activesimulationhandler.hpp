#pragma once

#include <QObject>

#include "api/isimulationdll.hpp"

#include "gui/properties/propertyadapter.hpp"


namespace simulationprovider
{

class ActiveSimulationHandler : public QObject
{
    Q_OBJECT

public:
    ActiveSimulationHandler(QString name, api::ISimulationDLL* simulationDll, QObject* parent = nullptr);

    const QString& name() const;
    const std::unique_ptr<gui::properties::PropertyAdapter>& properties() const;

private:
    void loadProperties();
    void createPropertyAdapter(QObject* property, gui::properties::PropertyAdapter* parent);

private:
    const QString m_name;
    api::ISimulationDLL* const m_simulationDll;
    std::unique_ptr<gui::properties::PropertyAdapter> m_propertyAdapters;
};

}  // namespace simulationprovider
