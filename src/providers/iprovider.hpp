#pragma once

#include <QObject>
#include <QFile>
#include <QDir>

#include "adapters/iadapter.hpp"


namespace providers
{

class IProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObjectList obtain READ obtain NOTIFY changed)

public:
    IProvider(QObject* parent) : QObject(parent) {}
    IProvider(const IProvider&) = delete;
    IProvider& operator=(const IProvider&) = delete;
    IProvider(IProvider&&) = delete;
    IProvider& operator=(IProvider&&) = delete;

    virtual QObjectList obtain() = 0;
    virtual adapters::IAdapter* select(const QString& name) = 0;

public slots:
    virtual void updateFromMap(const QVariantMap&) = 0;

signals:
    void changed();
};

}  // namespace providers
