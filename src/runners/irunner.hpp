#pragma once

#include <QObject>

#include "providers/iprovider.hpp"
#include "controllers/icontroller.hpp"
#include "api/simulation.hpp"


namespace runners
{

class IRunner : public QObject
{
    Q_OBJECT

public:
    IRunner(QObject* parent)
        : QObject(parent)
    {}
    IRunner(const IRunner&) = delete;
    IRunner& operator=(const IRunner&) = delete;
    IRunner(IRunner&&) = delete;
    IRunner& operator=(IRunner&&) = delete;

    virtual void setup(api::ISimulation* simulation,
                       std::shared_ptr<controllers::IController> controller,
                       providers::IProvider* statisticsToUpdate) = 0;
    virtual providers::IProvider* properties() = 0;
};

}  // namespace runners
