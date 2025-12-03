#pragma once

#include <QObject>
#include <QVariant>

#include "api/property.hpp"


namespace gui::properties
{

class PropertyAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString hint READ hint CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)      // "text" / "int" / "double" / "bool"
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool isGroup READ isGroup CONSTANT)
    Q_PROPERTY(QObjectList children READ children CONSTANT)

public:
    explicit PropertyAdapter(api::IProperty* prop, QObject* parent = nullptr)
        : QObject(parent)
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

    QObjectList children() const
    {
        return QObject::children();
    }

    bool isGroup() const
    {
        return m_prop && !m_prop->children().empty();
    }

public slots:
    void setValue(const QVariant& v)
    {
        if (!m_prop)
            return;
        if (!m_prop->set(v))
            emit valueChanged(m_prop->get());
    }

signals:
    void valueChanged(const QVariant& v);
    void valueClear();

private:
    api::IProperty* m_prop;
};

}  // namespace gui::properties
