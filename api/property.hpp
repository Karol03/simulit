#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <optional>
#include <QObject>
#include <QVariant>


namespace api
{

struct IProperty : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual const QString& name() const = 0;
    virtual const QString& description() const = 0;
    virtual QMetaType type() const = 0;

    virtual bool set(const QVariant& value) = 0;
    virtual QVariant get() const = 0;

    template <typename T>
    std::optional<T> getAs(const QString& propertyName) const
    {
        auto value = getByName(propertyName);
        if (!value.isNull() && value.template canConvert<T>())
            return value.template value<T>();
        return {};
    }
    virtual QVariant getByName(const QString& propertyName) const = 0;
};

template <typename T> inline bool convertQVariant(const QVariant& v);
template <> inline bool convertQVariant<int>(const QVariant& v)
{ bool ok = false; v.toInt(&ok); return ok; }
template <> inline bool convertQVariant<double>(const QVariant& v)
{ bool ok = false; v.toDouble(&ok); return ok; }
template <> inline bool convertQVariant<bool>(const QVariant& v)
{ return v.isValid(); }
template <> inline bool convertQVariant<QString>(const QVariant& v)
{ return v.isValid(); }

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
        if (convertQVariant<T>(value))
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

    QVariant getByName(const QString& propertyName) const override
    {
        if (m_name == propertyName)
            return m_data;
        return {};
    }

private:
    const QString m_name;
    const QString m_description;
    T m_default;
    T m_data;
    Filter m_filter;
};

using IPropertyPtr = std::unique_ptr<api::IProperty>;
using Properties = std::vector<IPropertyPtr>;

template <class... T>
concept AreProperties = requires { typename std::common_type_t<IPropertyPtr, T...>; };

template <class T>
concept PropertyType = requires { typename std::common_type_t<IPropertyPtr, T>; };

template <typename P>
static P&& withPropertyParent(P&& p, QObject* parent)
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
        , m_description{}
    {
        m_properties.reserve(sizeof...(Properties));
        (m_properties.emplace_back(
             withPropertyParent(std::forward<Properties>(properties), this)
             ), ...);
    }

    const QString& name() const final { return m_name; }
    const QString& description() const final { return m_description; }
    QMetaType type() const override { return QMetaType::fromType<std::nullptr_t>(); }

    bool set(const QVariant& value) final { return false; }
    QVariant get() const final { return QVariant{}; }

    QVariant getByName(const QString& propertyName) const override
    {
        for (const auto& property : m_properties)
        {
            if (auto result = property->getByName(propertyName); !result.isNull())
                return result;
        }
        return {};
    }

private:
    const QString m_name;
    const QString m_description;
    Properties m_properties;
};

template <typename T, typename... PropertyArgs>
inline auto prop(PropertyArgs&&... args)
{
    return std::make_unique<Property<T>>(std::forward<PropertyArgs>(args)...);
}

template <AreProperties... Properties>
inline auto prop(QObject* parent, Properties&&... childProperties)
{
    return std::make_unique<PropertyGroup>("", std::move(parent), std::forward<Properties>(childProperties)...);
}

template <AreProperties... Properties>
inline auto prop(QString groupName, Properties&&... childProperties)
{
    return std::make_unique<PropertyGroup>(std::move(groupName), nullptr, std::forward<Properties>(childProperties)...);
}

using Props = QVariantMap;
inline void parse(const QString& prefix, const IProperty* prop, Props& extracted)
{
    if (prop->children().empty())
    {
        extracted[QString("$1:$2").arg(prefix, prop->name())] = prop->get();
    }
    else
    {
        for (auto& child : prop->children())
        {
            if (auto* childProp = dynamic_cast<IProperty*>(child))
            {
                parse(QString("$1:$2").arg(prefix, prop->name()), childProp, extracted);
            }
        }
    }
}

template <PropertyType T>
inline auto parse(const T& props)
{
    auto result = Props{};
    parse("", props.get(), result);
    return result;
}

}  // namespace api
