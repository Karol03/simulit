#include "separatethread.hpp"

#include "providers/runnerproperties/separatethread.hpp"
#include "simpleworker.hpp"
#include "api/property.hpp"


namespace runners
{
SeparateThread::SeparateThread(QObject* parent)
    : IRunner(parent)
    , m_properties{ new providers::runnerproperties::SeparateThread(this) }
    , m_controller{nullptr}
    , m_statisticsToUpdate{nullptr}
    , m_worker{nullptr}
    , m_simulationThread{nullptr}
{}

SeparateThread::~SeparateThread()
{
    if (m_simulationThread)
    {
        m_controller->stop();
        m_simulationThread->quit();
        m_simulationThread->wait();
        m_simulationThread->deleteLater();
        m_controller.reset();
    }
}

void SeparateThread::setup(api::ISimulation* simulation,
                           std::shared_ptr<controllers::IController> controller,
                           providers::IProvider* statisticsToUpdate)
{
    if (m_simulationThread)
    {
        m_controller->stop();
        m_simulationThread->quit();
        m_simulationThread->wait();
        m_simulationThread->deleteLater();
        m_controller.reset();
        m_controller = nullptr;
        m_statisticsToUpdate = nullptr;
        m_simulationThread = nullptr;
        m_worker = nullptr;
    }

    if (simulation && controller && statisticsToUpdate)
    {
        auto rootProperty = m_properties->select("");
        auto propertyObject = rootProperty->raw();
        auto property = dynamic_cast<api::IProperty*>(propertyObject);
        auto parsedProperties = property->parse();
        auto config = SimpleWorkerConfiguration{
            parsedProperties["Przebieg::Liczba przebiegów"].toInt(),
            parsedProperties["Przebieg::Ziarno"].toInt(),
            parsedProperties["Przebieg::Opóźnienie"].toInt()
        };
        m_worker = new SimpleWorker(simulation, controller, config, nullptr);
        m_simulationThread = new QThread(this);

        // run simulation on thread start
        QObject::connect(m_simulationThread, &QThread::started, m_worker,  &IWorker::start);

        // update statistics on progress signal
        QObject::connect(simulation, &api::ISimulation::progress, statisticsToUpdate, &providers::IProvider::change);

        // deletes simulation on thread finish
        QObject::connect(m_simulationThread, &QThread::finished, simulation,  &QObject::deleteLater);

        //
        QObject::connect(controller.get(), &controllers::IController::started, m_worker,  &IWorker::start);

        m_simulationThread->start();
    }
}

providers::IProvider* SeparateThread::properties()
{
    return m_properties;
}

}  // namespace runners
