#pragma once

#include <QObject>
#include <QPointer>
#include <QVariant>

#include "api/variable.hpp"
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
    explicit Statistic(QString label,
                       QString hint,
                       api::VariableWatchList& watchedVariables,
                       int variableId,
                       QObject* parent)
        : IAdapter(parent)
        , m_watchedVariables{watchedVariables}
        , m_label{std::move(label)}
        , m_hint{std::move(hint)}
        , m_id{variableId}
    {}

    QString label() const
    {
        return m_label;
    }

    QString hint() const
    {
        return m_hint;
    }

    QVariant value() const
    {
        return m_watchedVariables[m_id];
    }

    void setValue(const QVariant&)
    {
        Q_ASSERT_X(false, "adapters::Statistic::setValue(const QVariant&)", "Setting stats from code is invalid");
    }

    QObjectList children() const
    {
        Q_ASSERT_X(false, "adapters::Statistic::children() const", "Calling children is invalid");
        return QObject::children();
    }

    QObject* raw()
    {
        Q_ASSERT_X(false, "adapters::Statistic::raw()", "Calling raw() is invalid");
        return nullptr;
    }

    const QObject* raw() const
    {
        Q_ASSERT_X(false, "adapters::Statistic::raw() const", "Calling raw() is invalid");
        return nullptr;
    }

    void updated()
    {
        emit changed(value());
    }

signals:
    void changed(const QVariant& v);

private:
    api::VariableWatchList& m_watchedVariables;
    const QString m_label;
    const QString m_hint;
    const int m_id;
};

}  // namespace adapters
