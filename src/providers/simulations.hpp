#pragma once

#include <QPointer>
#include "iprovider.hpp"


namespace providers
{

class Simulations : public IProvider
{
    Q_OBJECT

public:
    Simulations(QObject* parent);

    QObjectList obtain() override;
    adapters::IAdapter* select(const QString& name) override;

public slots:
    void change(const QVariantMap&) override { /* cannot modify simulation plugins, ignore */ }

private:
    QMap<QString, QObject*> m_simulations;
};

}  // namespace providers
