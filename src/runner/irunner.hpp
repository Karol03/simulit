#pragma once

#include <QObject>
#include <QFile>
#include <QDir>


class ISimulationController;


namespace loaders
{

class IRunner : public QObject
{
    Q_OBJECT

public:
    IRunner(QObject* parent) : QObject(parent) {}
    IRunner(const IRunner&) = delete;
    IRunner& operator=(const IRunner&) = delete;
    IRunner(IRunner&&) = delete;
    IRunner& operator=(IRunner&&) = delete;

    virtual QObject* load(const QFile& file) = 0;
    virtual QObjectList load(const QDir& directory) = 0;
};

}  // namespace loaders
