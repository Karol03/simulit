#include "constantproperties.hpp"

#include "../adapters/property.hpp"


namespace providers
{

ConstantProperties::ConstantProperties(QObject* parent)
    : IProvider(parent)
{
    m_rootProperty = api::prop(this,
         api::prop("Przebieg",
                   api::prop<int>("Liczba przebiegów", "Liczba powtórzeń symulacji, im większa, tym dokładniejsze wyniki <1, 10'000'000>", 1, [](const int& value) { return 0 < value && value <= 10'000'000; }),
                   api::prop<int>("Ziarno", "Ustalona wartość inicjalizująca generator losowy w celu powtarzalności wyników (random seed). Ustaw 0 dla losowego ziarna", 0, [](const int& value) { return true; }),
                   api::prop<int>("Opóźnienie", "Opóźnienie pomiędzy kolejnymi iteracjami (w milisekundach <0-3000>)", 20, [](const int& value) { return 0 <= value && value <= 3000; }))).release();
    traversalMapInsert(m_rootProperty);
    preorderTraversalSquash(m_rootProperty, m_squashedPropertyList);
}

QObjectList ConstantProperties::obtain()
{
    return m_squashedPropertyList;
}

adapters::IAdapter* ConstantProperties::select(const QString& name)
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

void ConstantProperties::change(const QVariantMap& values)
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

void ConstantProperties::traversalMapInsert(api::IProperty* property)
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

void ConstantProperties::preorderTraversalSquash(api::IProperty* property, QObjectList& result)
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
