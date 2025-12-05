#pragma once

#include <QMap>
#include <QSet>

#include "iloader.hpp"


namespace loaders
{

class DllLoader : public ILoader
{
    Q_OBJECT

public:
    using ILoader::ILoader;

    QObject* load(const QFile& file) override;
    QObjectList load(const QDir& directory) override;

private:
    QObject* loadPlugin(const QString& path);

private:
    inline static QMap<QString, QObject*> m_loadedPlugins = {};
    inline static QSet<QString> m_uniquePluginNames = {};
};

}  // namespace loaders
