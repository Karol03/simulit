#pragma once

#include <functional>
#include <QObject>
#include <QVariant>

#include "inamed.hpp"
#include "utils.hpp"


namespace api
{

class IProperty : public common::IHierarchicalNamedVariable
{
    Q_OBJECT

public:
    using common::IHierarchicalNamedVariable::IHierarchicalNamedVariable;
    virtual const QString& name() const override = 0;
    virtual const QString& description() const override = 0;
    virtual QMetaType type() const override = 0;
    virtual bool set(const QVariant& value) override = 0;
    virtual QVariant get() const override = 0;
    virtual QList<IHierarchicalNamedVariable*> inner() override = 0;
};


template <typename T>
struct Property : public IProperty
{
    using Filter = std::function<bool(const T&)>;

public:
    Property(QString name,
             QString description,
             T defaultValue = T{},
             Filter filter = [](const T&) { return true; },
             QObject* parent = nullptr)
        : IProperty(parent)
        , m_name{std::move(name)}
        , m_description{std::move(description)}
        , m_default{defaultValue}
        , m_data{defaultValue}
        , m_filter{std::move(filter)}
    {}

    const QString& name() const override
    {
        return m_name;
    }

    const QString& description() const override
    {
        return m_description;
    }

    QMetaType type() const override
    {
        return QMetaType::fromType<T>();
    }

    bool set(const QVariant& value) override
    {
        if (utils::convert<T>(value))
        {
            auto newValue = value.value<T>();
            if (m_filter(newValue))
            {
                m_data = newValue;
                return true;
            }
        }
        return false;
    }

    QVariant get() const override
    {
        return QVariant{m_data};
    }

    QList<IHierarchicalNamedVariable*> inner() override { return {}; }

private:
    const QString m_name;
    const QString m_description;
    T m_default;
    T m_data;
    Filter m_filter;
};


class PropertyGroup : public IProperty
{
    Q_OBJECT

public:
    template <class... Properties>
    PropertyGroup(QString name, QObject* parent, Properties&&... properties)
        : IProperty(parent)
        , m_name{std::move(name)}
        , m_description{}
    {
        m_properties.reserve(sizeof...(Properties));
        (m_properties.emplace_back(
             utils::withParent(std::forward<Properties>(properties), this)
             ), ...);
    }

    const QString& name() const final { return m_name; }
    const QString& description() const final { return m_description; }
    QMetaType type() const override { return QMetaType(QMetaType::Nullptr); }
    bool set(const QVariant& value) final { return false; }
    QVariant get() const final { return QVariant{}; }
    QList<IHierarchicalNamedVariable*> inner() override { return m_properties; }

private:
    const QString m_name;
    const QString m_description;
    QList<IHierarchicalNamedVariable*> m_properties;
};

template <class T>
concept PropertyType =
    (std::is_pointer_v<std::remove_cvref_t<T>> &&
     std::derived_from<std::remove_pointer_t<std::remove_cvref_t<T>>, IProperty>);

template <class... T>
concept PropertyTypes = (PropertyType<T> && ...);

template <typename T, typename... PropertyArgs>
inline auto prop(PropertyArgs&&... args)
{
    return new Property<T>(std::forward<PropertyArgs>(args)...);
}

template <PropertyTypes... Properties>
inline auto prop(QObject* parent, Properties&&... childProperties)
{
    return new PropertyGroup("", parent, std::forward<Properties>(childProperties)...);
}

template <PropertyTypes... Properties>
inline auto prop(QString groupName, Properties&&... childProperties)
{
    return new PropertyGroup(std::move(groupName), nullptr, std::forward<Properties>(childProperties)...);
}

template <PropertyTypes... Properties>
inline auto prop(QString groupName, QObject* parent, Properties&&... childProperties)
{
    return new PropertyGroup(std::move(groupName), parent, std::forward<Properties>(childProperties)...);
}

}  // namespace api
