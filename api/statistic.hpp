#pragma once

#include <memory>
#include <vector>
#include <QObject>
#include <QVariant>


namespace api
{

struct IStatistic : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QVariant get() const = 0;
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

    QString name() const override
    {
        return m_name;
    }

    QString description() const override
    {
        return m_description;
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
};

using IStatisticPtr = std::unique_ptr<api::IStatistic>;
using Statistics = std::vector<IStatisticPtr>;

template <class... T>
concept AreStatistic = requires { typename std::common_type_t<IStatisticPtr, T...>; };

template <class T>
concept StatType = requires { typename std::common_type_t<IStatisticPtr, T>; };

template <typename P>
static P&& withStatisticParent(P&& p, QObject* parent)
{
    if (p) p->setParent(parent);
    return std::forward<P>(p);
}

class StatisticGroup : public IStatistic
{
public:
    template <AreStatistic... Statistics>
    StatisticGroup(QString name, QObject* parent, Statistics&&... statistics)
        : IStatistic(parent)
        , m_name{std::move(name)}
    {
        m_statistic.reserve(sizeof...(Statistics));
        (m_statistic.emplace_back(
             withStatisticParent(std::forward<Statistics>(statistics), this)
             ), ...);
    }

    QString name() const final { return m_name; }
    QString description() const final { return ""; }
    QVariant get() const final { return ""; }

private:
    QString m_name;
    Statistics m_statistic;
};

template <typename T, typename... StatisticArgs>
inline auto stat(StatisticArgs&&... args)
{
    return std::make_unique<Statistic<T>>(std::forward<StatisticArgs>(args)...);
}

template <AreStatistic... Statistics>
inline auto stat(QObject* parent, Statistics&&... childStatistics)
{
    return std::make_unique<StatisticGroup>("", std::move(parent), std::forward<Statistics>(childStatistics)...);
}

template <AreStatistic... Statistics>
inline auto stat(QString groupName, Statistics&&... childStatistics)
{
    return std::make_unique<StatisticGroup>(std::move(groupName), nullptr, std::forward<Statistics>(childStatistics)...);
}

// class Stat
// {

// };

class Stats
{
public:
    // operator std::unordered_map<QString, Stat>()
    // {

    // }
};

inline void parse(const QString& prefix, const IStatistic* stat, Stats& extracted)
{
    // if (stat->children().empty())
    // {
    //     extracted[QString("$1:$2").arg(prefix, stat->name())] = stat->get();
    // }
    // else
    // {
    //     for (auto& child : prop->children())
    //     {
    //         if (auto* childProp = dynamic_cast<IProperty*>(child))
    //         {
    //             parse(QString("$1:$2").arg(prefix, prop->name()), childProp, extracted);
    //         }
    //     }
    // }
}

template <StatType T>
inline auto parse(const T& stats)
{
    auto result = Stats{};
    parse("", stats.get(), result);
    return result;
}

}  // namespace api
