#pragma once

#include <QVariant>


namespace api::utils
{

template <typename T> inline bool convert(const QVariant& v);
template <> inline bool convert<int>(const QVariant& v)
{ bool ok = false; v.toInt(&ok); return ok; }
template <> inline bool convert<unsigned>(const QVariant& v)
{ bool ok = false; v.toUInt(&ok); return ok; }
template <> inline bool convert<double>(const QVariant& v)
{ bool ok = false; v.toDouble(&ok); return ok; }
template <> inline bool convert<bool>(const QVariant& v)
{ return v.isValid(); }
template <> inline bool convert<QString>(const QVariant& v)
{ return v.isValid(); }


template <typename P>
inline P&& withParent(P&& p, QObject* parent)
{
    if (p) p->setParent(parent);
    return std::forward<P>(p);
}


struct BindedType
{
    BindedType(QVariant& storage)
        : m_storage(storage)
    {}
    BindedType(const BindedType&) = delete;
    BindedType& operator=(const BindedType&) = delete;
    BindedType(BindedType&& b) = delete;
    BindedType& operator=(BindedType&& b) = delete;
    virtual ~BindedType() = default;

protected:
    QVariant& m_storage;
};


template <typename T>
struct BindedTypeT : public BindedType
{
    using BindedType::BindedType;

    void operator=(const T& value) { m_storage = QVariant::fromValue<T>(value); }
    operator T() const { return m_storage.value<T>(); }
    T get() const { return static_cast<T>(*this); }
    void set(const T& value) { *this = value; }
};


struct Bool : public BindedTypeT<bool>
{
    using BindedTypeT<bool>::BindedTypeT;
};


struct String : public BindedTypeT<QString>
{
    using BindedTypeT<QString>::BindedTypeT;
};


template <typename T>
struct Numeric : public BindedTypeT<T>
{
    using BindedTypeT<T>::BindedTypeT;

    operator bool() const { return this->m_storage.template value<T>() != 0; }
    Numeric<T>& operator++() { this->m_storage = this->m_storage.template value<T>() + static_cast<T>(1); return *this; }
    Numeric<T>& operator--() { this->m_storage = this->m_storage.template value<T>() - static_cast<T>(1); return *this; }
    Numeric<T>& operator+=(const Numeric<T>& value) { this->m_storage = this->m_storage.template value<T>() + value; return *this; }
    Numeric<T>& operator+=(const T& value) { this->m_storage = this->m_storage.template value<T>() + value; return *this; }
    Numeric<T>& operator-=(const Numeric<T>& value) { this->m_storage = this->m_storage.template value<T>() - value; return *this; }
    Numeric<T>& operator-=(const T& value) { this->m_storage = this->m_storage.template value<T>() - value; return *this; }
    Numeric<T>& operator*=(const Numeric<T>& value) { this->m_storage = this->m_storage.template value<T>() * value; return *this; }
    Numeric<T>& operator*=(const T& value) { this->m_storage = this->m_storage.template value<T>() * value; return *this; }
    Numeric<T>& operator/=(const Numeric<T>& value) { this->m_storage = this->m_storage.template value<T>() / value; return *this; }
    Numeric<T>& operator/=(const T& value) { this->m_storage = this->m_storage.template value<T>() / value; return *this; }
};


struct Int : public Numeric<int>
{
    using Numeric<int>::Numeric;
    friend auto operator<=>(const Int& lhs, const Int& rhs)
    {
        return lhs.m_storage.toInt() <=> rhs.m_storage.toInt();
    }
};


struct Unsigned : public Numeric<unsigned>
{
    using Numeric<unsigned>::Numeric;
    friend auto operator<=>(const Unsigned& lhs, const Unsigned& rhs)
    {
        return lhs.m_storage.toUInt() <=> rhs.m_storage.toUInt();
    }
};


struct Double : public Numeric<double>
{
    using Numeric<double>::Numeric;
    friend auto operator<=>(const Double& lhs, const Double& rhs)
    {
        return lhs.m_storage.toDouble() <=> rhs.m_storage.toDouble();
    }
};


template<typename T> struct BindingWrapper;
template<> struct BindingWrapper<bool> { using type = Bool; };
template<> struct BindingWrapper<int> { using type = Int; };
template<> struct BindingWrapper<unsigned> { using type = Unsigned; };
template<> struct BindingWrapper<double> { using type = Double; };
template<> struct BindingWrapper<QString> { using type = String; };


class BindingMap
{
public:
    BindingMap()
    {}

    BindingMap(QVariantMap map)
        : m_data{std::move(map)}
    {}

    void reinitialize(QVariantMap map)
    {
        std::swap(m_data, map);
    }

    bool contains(const QString& name) const
    {
        return m_data.contains(name);
    }

    template <typename T>
    auto ref(const QString& name)
    {
        using Wrapper = typename BindingWrapper<T>::type;
        if (!m_data.contains(name))
            throw std::runtime_error("Property does exists in the BindingMap");
        if (!utils::convert<T>(m_data[name]))
            throw std::runtime_error("BindingMap invalid ref() usage. Cannot convert to given type");
        return Wrapper(m_data[name]);
    }

    operator QVariantMap&() { return m_data; }
    operator const QVariantMap&() const { return m_data; }

private:
    QVariantMap m_data;
};


}  // namespace api::utils
