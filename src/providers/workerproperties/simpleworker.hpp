#pragma once

#include <QPointer>

#include "providers/iprovider.hpp"
#include "api/property.hpp"


namespace providers::workerproperties
{

class SimpleWorker : public IProvider
{
    Q_OBJECT

public:
    SimpleWorker(QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void change(const QVariantMap&) override;

private:
    void traversalMapInsert(api::common::IHierarchicalNamedVariable* property);
    void preorderTraversalSquash(api::common::IHierarchicalNamedVariable* property,
                                 QObjectList& result);

private:
    api::IProperty* m_rootProperty;
    QMap<QString, QPointer<api::IProperty>> m_properties;
    QObjectList m_squashedPropertyList;
};

}  // namespace providers::workerproperties
