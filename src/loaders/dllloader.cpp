#include "dllloader.hpp"

#include <memory>
#include <QPluginLoader>

#include "api/simulation.hpp"


namespace loaders
{

QObject* DllLoader::loadPlugin(const QString& path)
{
    QPluginLoader loader(path);
    const auto meta = loader.metaData();

    const QString classNameToLoad = meta.value("className").toString();
    if (classNameToLoad.isEmpty())
        return nullptr;

    if (m_loadedPlugins.contains(classNameToLoad))
        return m_loadedPlugins[classNameToLoad];

    QObject* plugin = loader.instance();
    if (!plugin)
        return nullptr;

    if (auto simulationModule = qobject_cast<api::ISimulationDLL*>(plugin))
    {
        if (m_uniquePluginNames.contains(simulationModule->name()))
        {
            loader.unload();
            return nullptr;
        }
        m_loadedPlugins.insert(classNameToLoad, plugin);
        m_uniquePluginNames.insert(simulationModule->name());
        return plugin;
    }
    loader.unload();
    return nullptr;
}

QObject* DllLoader::load(const QFile& file)
{
    auto info = QFileInfo(file);
    if (info.exists() && info.isFile() && info.suffix().compare("dll", Qt::CaseInsensitive) == 0)
    {
        const bool allPlugins = false;
        return loadPlugin(info.absoluteFilePath());
    }
    return nullptr;
}

QObjectList DllLoader::load(const QDir& directory)
{
    QObjectList dlls;
    QStringList filters{QString{"*.dll"}};
    for (const QString& fileName : directory.entryList(filters, QDir::Files))
    {
        auto path = directory.absoluteFilePath(fileName);
        if (auto* plugin = loadPlugin(path))
        {
            dlls.append(plugin);
        }
    }
    return dlls;
}

}  // namespace loaders
