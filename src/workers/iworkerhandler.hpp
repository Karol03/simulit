#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "controllers/icontroller.hpp"


namespace workers
{

/**
 * @brief The IWorkerHandler class
 * It handles simulations in a separate thread.
 * The user can manage the simulation through the controller() object.
 * To add a new handler, do not forget register it in the
 *   WorkerHandlerFactory constructor
 */
class IWorkerHandler : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual providers::IProvider* ownProperties() = 0;
    virtual providers::IProvider* simulationProperties() = 0;
    virtual providers::IProvider* statistics() = 0;
    virtual controllers::IController* controller() = 0;
};

}  // namespace workers
