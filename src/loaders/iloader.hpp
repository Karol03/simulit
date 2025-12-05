#pragma once

#include <QObject>
#include <QFile>
#include <QDir>


namespace loaders
{

class ILoader : public QObject
{
    Q_OBJECT

public:
    ILoader(QObject* parent) : QObject(parent) {}
    ILoader(const ILoader&) = delete;
    ILoader& operator=(const ILoader&) = delete;
    ILoader(ILoader&&) = delete;
    ILoader& operator=(ILoader&&) = delete;

    virtual QObject* load(const QFile& file) = 0;
    virtual QObjectList load(const QDir& directory) = 0;
};

}  // namespace loaders
