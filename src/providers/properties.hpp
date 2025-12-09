#pragma once

#include <QPointer>

#include "iprovider.hpp"
#include "api/variable.hpp"


namespace providers
{

class Properties : public IProvider
{
    Q_OBJECT

public:
    Properties(api::Variables properties, QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void updateFromMap(const QVariantMap& update) override;

private:
    void createAdapters(api::Variables properties);

private:
    QObjectList m_adapters;
};

}  // namespace providers
