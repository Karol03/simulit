#pragma once

#include <QObject>
#include <QVariant>

#include "api/statistic.hpp"


namespace gui::statistics
{

class StatisticAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString hint READ hint CONSTANT)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QObjectList children READ children CONSTANT)

public:
    explicit StatisticAdapter(api::IStatistic* stat, QObject* parent = nullptr)
        : QObject(parent)
        , m_stat(stat)
    {
        Q_ASSERT(m_stat);
        if (auto statGroup = dynamic_cast<api::StatisticGroup*>(m_stat))
        {
            for (const auto& child : statGroup->children())
            {
                m_statsById[child->id()] = new StatisticAdapter(child.get(), this);
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

    void setValue(QVariant value)
    {
        m_stat->set(value);
        emit valueChanged(value);
    }

    QObjectList children() const
    {
        return QObject::children();
    }

    void update(const std::unordered_map<int, QVariant>& statsToUpdate)
    {
        for (const auto& [id, value] : statsToUpdate)
        {
            m_statsById.at(id)->setValue(value);
        }
    }

signals:
    void valueChanged(const QVariant& v);

private:
    api::IStatistic* m_stat;
    std::unordered_map<int, StatisticAdapter*> m_statsById;
};

}  // namespace gui::statistics
