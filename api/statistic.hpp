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
    IStatistic(QObject* parent = nullptr)
        : m_id{++m_statId}
    {}
    IStatistic(const IStatistic&) = delete;
    IStatistic& operator=(const IStatistic&) = delete;
    IStatistic(IStatistic&&) = delete;
    IStatistic& operator=(IStatistic&&) = delete;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QVariant get() const = 0;
    virtual bool set(const QVariant& value) = 0;
    virtual int id() const { return m_id; }

protected:
    const int m_id;

private:
    inline static int m_statId = 0;
};

template <typename T> inline bool convertQVariantStat(const QVariant& v);
template <> inline bool convertQVariantStat<int>(const QVariant& v)
{ bool ok = false; v.toInt(&ok); return ok; }
template <> inline bool convertQVariantStat<double>(const QVariant& v)
{ bool ok = false; v.toDouble(&ok); return ok; }
template <> inline bool convertQVariantStat<bool>(const QVariant& v)
{ return v.isValid(); }
template <> inline bool convertQVariantStat<QString>(const QVariant& v)
{ return v.isValid(); }

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

    bool set(const QVariant& value) override
    {
        if (convertQVariantStat<T>(value))
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
    StatisticGroup(QObject* parent, Statistics&&... statistics)
        : IStatistic(parent)
    {
        m_statistic.reserve(sizeof...(Statistics));
        (m_statistic.emplace_back(
             withStatisticParent(std::forward<Statistics>(statistics), this)
             ), ...);
    }

    QString name() const final { return ""; }
    QString description() const final { return ""; }
    QVariant get() const final { return ""; }
    bool set(const QVariant& value) final { return false; }
    const Statistics& children() { return m_statistic; }

private:
    Statistics m_statistic;
};

template <typename T, typename... StatisticArgs>
inline auto stat(StatisticArgs&&... args)
{
    return std::make_unique<Statistic<T>>(std::forward<StatisticArgs>(args)...);
}

template <AreStatistic... Statistics>
inline auto stat(Statistics&&... childStatistics)
{
    return std::make_unique<StatisticGroup>(nullptr, std::forward<Statistics>(childStatistics)...);
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
