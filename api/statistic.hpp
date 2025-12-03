#pragma once

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
    virtual QString value() const = 0;
    virtual void resetValue() = 0;
};

struct SimpleStatistic : public IStatistic
{
public:
    SimpleStatistic(QString name, QString description)
        : m_name{std::move(name)}
        , m_description{std::move(description)}
    {}

    SimpleStatistic(QString name, QString description, QString value)
        : m_name{std::move(name)}
        , m_description{std::move(description)}
        , m_value{std::move(value)}
    {}

    QString name() const override { return m_name; }
    QString description() const override { return m_name; }
    QString value() const override { return m_name; }
    void reset() { m_value = ""; }

private:
    QString m_name;
    QString m_description;
    QString m_value;
};

using IStatisticPtr = std::unique_ptr<api::SimpleStatistic>;
using Statistics = std::vector<IStatisticPtr>;

template <class... T>
concept AreStatistic = requires { typename std::common_type_t<IStatisticPtr, T...>; };

class StatisticGroup : public IStatistic
{
public:
    template <AreStatistic... Statistic>
    StatisticGroup(Statistic&&... statistic)
        : m_statistic{std::forward<Statistic>(statistic)...}
    {}

    template <AreStatistic... Statistic>
    StatisticGroup(QString name, Statistic&&... statistic)
        : m_name{std::move(name)}
        , m_statistic{std::forward<Statistic>(statistic)...}
    {}

    QString name() const final { return m_name; }
    QString description() const final { return ""; }
    QString value() const final { return ""; }

private:
    QString m_name;
    Statistics m_statistic;
};

template <typename... Args>
inline auto CreateStatistic(Args&&... args)
{
    return std::make_unique<SimpleStatistic>(std::forward<Args>(args)...);
}

template <typename... Args>
inline auto CreateStatisticGroup(Args&&... args)
{
    return std::make_unique<StatisticGroup>(std::forward<Args>(args)...);
}

}  // namespace api
