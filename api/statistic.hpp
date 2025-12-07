#pragma once

#include <QObject>
#include <QVariant>

#include "inamed.hpp"
#include "utils.hpp"


namespace api
{

class IStatistic : public common::IHierarchicalNamedVariable
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
struct Statistic : public IStatistic
{
public:
    Statistic(QString name,
              QString description,
              T beginValue,
              QObject* parent = nullptr)
        : IStatistic(parent)
        , m_name{std::move(name)}
        , m_description{std::move(description)}
        , m_default{beginValue}
        , m_data{beginValue}
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

    QVariant get() const override
    {
        return QVariant{m_data};
    }

    bool set(const QVariant& value) override
    {
        if (utils::convert<T>(value))
        {
            m_data = value.value<T>();
            return true;
        }
        return false;
    }

    QList<IHierarchicalNamedVariable*> inner() override { return {}; }

private:
    QString m_name;
    QString m_description;
    T m_default;
    T m_data;
};


class StatisticGroup : public IStatistic
{
    Q_OBJECT

public:
    template <class... Statistics>
    StatisticGroup(QObject* parent, Statistics&&... statistics)
        : IStatistic(parent)
        , m_name{}
        , m_description{}
    {
        m_statistic.reserve(sizeof...(Statistics));
        (m_statistic.emplace_back(
             withParent(std::forward<Statistics>(statistics), this)
             ), ...);
    }

    const QString& name() const final { return m_name; }
    const QString& description() const final { return m_description; }
    QMetaType type() const override { return QMetaType(QMetaType::Nullptr); }
    QVariant get() const final { return ""; }
    bool set(const QVariant& value) final { return false; }
    QList<IHierarchicalNamedVariable*> inner() override { return m_statistic; }

private:
    const QString m_name;
    const QString m_description;
    QList<IHierarchicalNamedVariable*> m_statistic;
};

template <class T>
concept StatisticType =
    (std::is_pointer_v<std::remove_cvref_t<T>> &&
     std::derived_from<std::remove_pointer_t<std::remove_cvref_t<T>>, IStatistic>);

template <class... T>
concept StatisticTypes = (StatisticType<T> && ...);

template <typename T, typename... StatisticArgs>
inline auto stat(StatisticArgs&&... args)
{
    return new Statistic<T>(std::forward<StatisticArgs>(args)...);
}

template <StatisticTypes... Statistics>
inline auto stat(Statistics&&... childStatistics)
{
    return new StatisticGroup(nullptr, std::forward<Statistics>(childStatistics)...);
}

}  // namespace api
