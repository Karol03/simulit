#pragma once

#include <QObject>
#include <QPointer>
#include <QVariant>

#include "api/statistic.hpp"
#include "iadapter.hpp"


namespace adapters
{

class Statistic : public IAdapter
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString hint READ hint CONSTANT)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(QObjectList children READ children CONSTANT)

public:
    explicit Statistic(api::IStatistic* stat, QObject* parent)
        : IAdapter(parent)
        , m_stat(stat)
    {
        Q_ASSERT(m_stat);

        if (auto statGroup = dynamic_cast<api::StatisticGroup*>(stat))
        {
            for (const auto& child : statGroup->children())
            {
                new Statistic(child.get(), this);
            }
        }
    }

    QString label() const
    {
        return m_stat->name();
    }

    QString hint() const
    {
        return m_stat->description();
    }

    QVariant value() const
    {
        return m_stat->get();
    }

    void setValue(const QVariant& v)
    {
        if (!m_stat)
            return;
        if (!m_stat->set(v))
            emit changed(m_stat->get());
    }

    QObjectList children() const
    {
        return QObject::children();
    }

    QObject* raw() override
    {
        return m_stat;
    }

    const QObject* raw() const override
    {
        return m_stat;
    }

signals:
    void changed(const QVariant& v);

private:
    QPointer<api::IStatistic> m_stat;
};

}  // namespace adapters
