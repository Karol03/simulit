#pragma once

#include <QObject>
#include <QPointer>
#include <QVariant>

#include "api/property.hpp"
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
    Q_PROPERTY(QObjectList children READ children CONSTANT)

public:
    explicit Property(api::IProperty* prop, QObject* parent)
        : IAdapter(parent)
        , m_prop(prop)
    {
        Q_ASSERT(m_prop);

        if (auto propertyGroup = dynamic_cast<api::PropertyGroup*>(prop))
        {
            for (const auto& child : propertyGroup->children())
            {
                new adapters::Property(child.get(), this);
            }
        }
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

    QObjectList children() const
    {
        return QObject::children();
    }

    bool isGroup() const
    {
        return !QObject::children().empty();
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
    QPointer<api::IProperty> m_prop;
};

}  // namespace adapters
