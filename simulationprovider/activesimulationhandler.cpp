#include "activesimulationhandler.hpp"


namespace simulationprovider
{

ActiveSimulationHandler::ActiveSimulationHandler(QString name,
                                                 api::ISimulationDLL* simulationDll,
                                                 QObject* parent)
    : QObject{parent}
    , m_name{std::move(name)}
    , m_simulationDll{std::move(simulationDll)}
{
    loadProperties();
}

const QString& ActiveSimulationHandler::name() const
{
    return m_name;
}

void ActiveSimulationHandler::loadProperties()
{
    if (auto& propertyApi = m_simulationDll->properties())
    {
        m_propertyAdapters = std::make_unique<gui::properties::PropertyAdapter>(propertyApi.get());
        for (auto* child : propertyApi->children())
        {
            createPropertyAdapter(child, m_propertyAdapters.get());
        }
    }
}

void ActiveSimulationHandler::createPropertyAdapter(QObject* propertyOobject,
                                                    gui::properties::PropertyAdapter* parent)
{
    if (auto* property = dynamic_cast<api::IProperty*>(propertyOobject))
    {
        auto adapter = new gui::properties::PropertyAdapter(property, parent);
        for (auto* child : property->children())
        {
            createPropertyAdapter(child, adapter);
        }
    }
}

const std::unique_ptr<gui::properties::PropertyAdapter>& ActiveSimulationHandler::properties() const
{
    return m_propertyAdapters;
}

}  // namespace simulationprovider
