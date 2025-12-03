#pragma once

#include <functional>
#include <memory>
#include <QObject>
#include <QVariant>


namespace api
{

struct IProperty : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QMetaType type() const = 0;

    virtual bool set(const QVariant& value) = 0;
    virtual QVariant get() const = 0;
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

    QString name() const override
    {
        return m_name;
    }

    QString description() const override
    {
        return m_description;
    }

    QMetaType type() const override
    {
        return QMetaType::fromType<T>();
    }

    bool set(const QVariant& value) override
    {
        if (value.canConvert<T>())
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

private:
    QString m_name;
    QString m_description;
    T m_default;
    T m_data;
    Filter m_filter;
};

using IPropertyPtr = std::unique_ptr<api::IProperty>;
using Properties = std::vector<IPropertyPtr>;

template <class... T>
concept AreProperties = requires { typename std::common_type_t<IPropertyPtr, T...>; };

template <typename P>
static P&& withParent(P&& p, QObject* parent)
{
    if (p) p->setParent(parent);
    return std::forward<P>(p);
}

class PropertyGroup : public IProperty
{
public:
    template <AreProperties... Properties>
    PropertyGroup(QString name, QObject* parent, Properties&&... properties)
        : IProperty(parent)
        , m_name{std::move(name)}
    {
        m_properties.reserve(sizeof...(Properties));
        (m_properties.emplace_back(
             withParent(std::forward<Properties>(properties), this)
             ), ...);
    }

    QString name() const final { return m_name; }
    QString description() const final { return ""; }
    QMetaType type() const override { return QMetaType::fromType<std::nullptr_t>(); }

    bool set(const QVariant& value) final { return false; }
    QVariant get() const final { return QVariant{}; }

private:
    QString m_name;
    Properties m_properties;
};

template <typename T, typename... PropertyArgs>
inline auto CreateProperty(PropertyArgs&&... args)
{
    return std::make_unique<Property<T>>(std::forward<PropertyArgs>(args)...);
}

template <AreProperties... Properties>
inline auto CreateProperty(QObject* parent, Properties&&... childProperties)
{
    return std::make_unique<PropertyGroup>("", std::move(parent), std::forward<Properties>(childProperties)...);
}

template <AreProperties... Properties>
inline auto CreateProperty(QString groupName, QObject* parent, Properties&&... childProperties)
{
    return std::make_unique<PropertyGroup>(std::move(groupName), std::move(parent), std::forward<Properties>(childProperties)...);
}

}  // namespace api
