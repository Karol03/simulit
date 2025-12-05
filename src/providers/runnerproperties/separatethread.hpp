#pragma once

#include <QPointer>

#include "providers/iprovider.hpp"
#include "api/property.hpp"


namespace providers::runnerproperties
{

class SeparateThread : public IProvider
{
    Q_OBJECT

public:
    SeparateThread(QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void change(const QVariantMap&) override;

private:
    void traversalMapInsert(api::IProperty* property);
    void preorderTraversalSquash(api::IProperty* property, QObjectList& result);

private:
    api::IProperty* m_rootProperty;
    QMap<QString, QPointer<api::IProperty>> m_properties;
    QObjectList m_squashedPropertyList;
};

}  // namespace providers::runnerproperties
