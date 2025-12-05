#pragma once

#include <QPointer>

#include "iprovider.hpp"
#include "api/property.hpp"
#include "api/simulation.hpp"


namespace providers
{

class Properties : public IProvider
{
    Q_OBJECT

public:
    Properties(api::ISimulationDLL& simulation, QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void change(const QVariantMap&) override;

private:
    void traversalMapInsert(api::IProperty* property);
    void preorderTraversalSquash(api::IProperty* property, QObjectList& result);

private:
    QPointer<api::IProperty> m_rootProperty;
    QMap<QString, QPointer<api::IProperty>> m_properties;
};

}  // namespace providers
