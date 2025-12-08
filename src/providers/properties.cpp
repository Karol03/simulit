#include "properties.hpp"

#include "adapters/property.hpp"


namespace providers
{

Properties::Properties(api::ISimulationDLL& simulation, QObject* parent)
    : IProvider(parent)
{
    if (simulation.properties())
        createAdapters(simulation.properties());
}

QObjectList Properties::obtain()
{
    return m_adapters;
}

adapters::IAdapter* Properties::select(const QString& name)
{
    // select any Properties do nothing
    return nullptr;
}

void Properties::createAdapters(api::Variables properties)
{
    QObjectList variablesWithName;
    properties->preorderTraversalSquash(
        variablesWithName,
        [](const auto& variable) { return !variable.name().isEmpty(); });
    m_adapters.reserve(variablesWithName.size());
    for (auto& property : variablesWithName)
    {
        m_adapters.append(new adapters::Property(dynamic_cast<api::IVariable*>(property), this));
    }
}

void Properties::updateFromMap(const QVariantMap& update)
{
    /* properties updated only from GUI, ignore */
}

}  // namespace providers
