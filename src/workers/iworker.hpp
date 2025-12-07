#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "controllers/icontroller.hpp"
#include "api/simulation.hpp"


namespace workers
{

class IWorker : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    IWorker(const IWorker&) = delete;
    IWorker& operator=(const IWorker&) = delete;
    IWorker(IWorker&&) = delete;
    IWorker& operator=(IWorker&&) = delete;

    virtual void setup(api::ISimulationDLL* plugin) = 0;
    virtual void terminate() = 0;
};

}  // namespace workers
