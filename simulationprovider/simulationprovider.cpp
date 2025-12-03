#include "simulationprovider.hpp"

#include <QDir>
#include <QPluginLoader>


namespace simulationprovider
{

SimulationProvider::SimulationProvider(QObject* parent)
    : QObject(parent)
{}

void SimulationProvider::loadSimulations()
{
    QDir dir("simulations");
    QStringList filters{QString{"*.dll"}};
    QStringList errors;

    for (const QString& fileName : dir.entryList(filters, QDir::Files))
    {
        QPluginLoader loader(dir.absoluteFilePath(fileName));

        const auto meta = loader.metaData();
        const QString className = meta.value("className").toString();
        if (className.isEmpty())
        {
            errors << QString("Nie udało się załadować symulacji %1").arg(fileName);
            continue;
        }

        if (m_loadedSimulations.contains(className))
        {
            errors << QString("Symulacja '%1' została już załadowana").arg(fileName);
            continue;
        }

        QObject* plugin = loader.instance();
        if (!plugin)
            continue;

        if (auto simulationModule = qobject_cast<api::ISimulationDLL*>(plugin))
        {
            if (m_simulations.contains(simulationModule->name()))
            {
                errors << QString("Symulacja o nazwie '%1' już istnieje").arg(simulationModule->name());
            }
            else
            {
                m_loadedSimulations.insert(className);
                m_simulations[simulationModule->name()] = simulationModule;
            }
        }
    }

    if (!errors.empty())
        emit errorOccurred(errors.join('\n'));
    emit simulationsChanged();
}

void SimulationProvider::select(QString name)
{
    if (m_activeSimulationHandler && m_activeSimulationHandler->name() == name)
        return;
    m_activeSimulationHandler = std::make_unique<ActiveSimulationHandler>(name, m_simulations[name]);
    emit propertiesChanged();
}

QStringList SimulationProvider::simulations() const
{
    QStringList list;
    for (const auto& [name, _] : m_simulations)
    {
        list << name;
    }
    return list;
}

QObjectList SimulationProvider::properties() const
{
    auto result = QObjectList();
    if (m_activeSimulationHandler && m_activeSimulationHandler->properties() &&
        !m_activeSimulationHandler->properties()->children().empty())
    {
        return m_activeSimulationHandler->properties()->children();
    }
    return result;
}

QObjectList SimulationProvider::statistics() const
{
    return QObjectList();
}

}  // namespace simulationprovider
