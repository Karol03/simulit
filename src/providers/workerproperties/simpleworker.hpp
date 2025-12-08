#pragma once

#include <QPointer>

#include "providers/iprovider.hpp"
#include "api/variable.hpp"


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
    void updateFromMap(const QVariantMap& update) override;

private:
    void createAdapters();

private:
    api::IVariable* m_rootProperty;
    QObjectList m_adapters;
};

}  // namespace providers::workerproperties
