#include "pool.hpp"

#include "workerhandlerfactory.hpp"


namespace workers
{

Pool::Pool(QObject* parent)
    : QObject(parent)
    , m_workers{}
{}

bool Pool::exists(QString name)
{
    return m_workers.contains(name);
}

IWorkerHandler* Pool::operator[](QString name)
{
    if (m_workers.contains(name))
        return m_workers[name];
    return nullptr;
}

IWorkerHandler* Pool::create(QString name, api::ISimulationDLL* plugin)
{
    if (m_workers.contains(name))
        return nullptr;

    if (m_usedPlugins.contains(plugin))
        return nullptr;

    if (auto handler = WorkerHandlerFactory(this).createFrom(plugin))
    {
        handler->setParent(this);
        m_usedPlugins.insert(plugin);
        m_workers.insert(name, handler);
        return handler;
    }
    return nullptr;

}

void Pool::terminate(QString name)
{
    m_workers.remove(name);
}

void Pool::terminate()
{
    m_workers.clear();
}

}  // namespace workers
