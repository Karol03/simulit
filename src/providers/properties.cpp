#include "properties.hpp"

#include "../adapters/property.hpp"


namespace providers
{

Properties::Properties(api::ISimulationDLL& simulation, QObject* parent)
    : IProvider(parent)
{
    if (!simulation.properties())
        return;
    m_rootProperty = simulation.properties().get();
    traversalMapInsert(m_rootProperty);
}

QObjectList Properties::obtain()
{
    auto result = QObjectList{};
    if (m_rootProperty)
        preorderTraversalSquash(m_rootProperty, result);
    return result;
}

adapters::IAdapter* Properties::select(const QString& name)
{
    if (name.isEmpty())
    {
        return new adapters::Property(m_rootProperty, this);
    }
    if (m_properties.contains(name))
    {
        return new adapters::Property(m_properties[name], this);
    }
    return nullptr;
}

void Properties::change(const QVariantMap& values)
{
    for (const auto& [name, value] : values.asKeyValueRange())
    {
        if (m_properties.contains(name) && m_properties[name])
        {
            m_properties[name]->set(value);
        }
    }
    emit changed();
};

void Properties::traversalMapInsert(api::IProperty* property)
{
    if (!property->name().isEmpty())
        m_properties[property->name()] = property;

    if (auto group = dynamic_cast<api::PropertyGroup*>(property))
    {
        for (const auto& child : group->children())
        {
            traversalMapInsert(child.get());
        }
    }
}

void Properties::preorderTraversalSquash(api::IProperty* property, QObjectList& result)
{
    if (!property->name().isEmpty())
        result.append(new adapters::Property(property, this));
    if (auto group = dynamic_cast<api::PropertyGroup*>(property))
    {
        for (const auto& child : group->children())
        {
            preorderTraversalSquash(child.get(), result);
        }
    }
}

}  // namespace providers
