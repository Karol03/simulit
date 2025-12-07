#pragma once

#include <QThread>
#include <QMap>
#include <QSet>

#include "api/simulation.hpp"
#include "iworkerhandler.hpp"


namespace workers
{


class Pool : public QObject
{
    Q_OBJECT

public:
    Pool(QObject* parent);

    bool exists(QString name);
    IWorkerHandler* operator[](QString name);
    IWorkerHandler* create(QString name, api::ISimulationDLL* plugin);
    void terminate(QString name);
    void terminate();

private:
    QMap<QString, IWorkerHandler*> m_workers;
    QSet<api::ISimulationDLL*> m_usedPlugins;
};

}  // namespace workers
