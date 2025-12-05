#pragma once

#include <QObject>
#include <QVariant>


namespace api::common
{

class INamedVariable : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual const QString& name() const = 0;
    virtual const QString& description() const = 0;
    virtual QMetaType type() const = 0;
    virtual bool set(const QVariant& value) = 0;
    virtual QVariant get() const = 0;
};


class IHierarchicalNamedVariable : public INamedVariable
{
    Q_OBJECT

public:
    using INamedVariable::INamedVariable;

    virtual const QString& name() const override = 0;
    virtual const QString& description() const override = 0;
    virtual QMetaType type() const override = 0;
    virtual bool set(const QVariant& value) override = 0;
    virtual QVariant get() const override = 0;

    // because of QObject::children, cannot name the method children()
    virtual QList<IHierarchicalNamedVariable*> inner() = 0;
    QVariantMap parse()
    {
        auto result = QVariantMap{};
        parse("", this, result);
        return result;
    }

private:
    void parse(const QString& prefix,
               IHierarchicalNamedVariable* variable,
               QVariantMap& extracted) const
    {
        QString currentKey = prefix.isEmpty() ? variable->name() : prefix + ":" + variable->name();
        if (variable->children().empty())
        {
            extracted[currentKey] = variable->get();
            return;
        }
        for (auto child : variable->inner())
        {
            parse(currentKey, child, extracted);
        }
    }
};

}  // namespace api::common
