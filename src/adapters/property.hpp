#pragma once

#include <QObject>
#include <QPointer>
#include <QVariant>

#include "api/variable.hpp"
#include "iadapter.hpp"


namespace adapters
{

class Property : public IAdapter
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString hint READ hint CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)      // "text" / "int" / "double" / "bool"
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY changed)
    Q_PROPERTY(bool isGroup READ isGroup CONSTANT)
    Q_PROPERTY(int groupDepth READ groupDepth CONSTANT)

public:
    explicit Property(api::IVariable* prop, QObject* parent)
        : IAdapter(parent)
        , m_prop(prop)
    {
        Q_ASSERT(m_prop);
    }

    QString label() const
    {
        return m_prop->name();
    }

    QString hint() const
    {
        return m_prop->description();
    }

    QVariant value() const
    {
        return m_prop->get();
    }

    QString type() const
    {
        const QMetaType mt = m_prop->type();
        switch (mt.id()) {
        case QMetaType::Int:
            return "int";
        case QMetaType::Double:
            return "double";
        case QMetaType::Bool:
            return "bool";
        case QMetaType::QString:
        default:
            return "text";
        }
    }

    bool isGroup() const
    {
        return m_prop && !m_prop->children().isEmpty();
    }

    int groupDepth() const
    {
        if (m_prop)
            return depthOf(m_prop);
        return 0;
    }

    QObject* raw() override
    {
        return m_prop;
    }

    const QObject* raw() const override
    {
        return m_prop;
    }

public slots:
    void setValue(const QVariant& v)
    {
        if (!m_prop)
            return;
        if (!m_prop->set(v))
            emit changed(m_prop->get());
    }

signals:
    void changed(const QVariant& v);

private:
    int depthOf(api::IVariable* var) const
    {
        if (!var->parent())
            return 0;
        if (auto parentVar = dynamic_cast<api::IVariable*>(var->parent()))
            return 1 + depthOf(parentVar);
        return 0;
    }

private:
    QPointer<api::IVariable> m_prop;
};

}  // namespace adapters
