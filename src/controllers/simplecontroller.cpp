#include "simplecontroller.hpp"

#include <QTimer>
#include "api/simulation.hpp"
#include "tools/numbergeneratorfactory.hpp"


namespace controllers
{

SimpleController::SimpleController(api::ISimulationDLL* plugin,
                                   providers::Statistics& statisticsToUpdate,
                                   QObject* parent)
    : IController(parent)
    , m_plugin{plugin}
    , m_statistics{statisticsToUpdate}
    , m_properties{nullptr}
    , m_simulationThread{nullptr}
    , m_state{ControllerState::Ready}
    , m_isBusy{false}
{
    m_properties = api::var("Przebieg", this,
                            api::var<int>("Liczba przebiegów", "Liczba powtórzeń symulacji, im większa, tym dokładniejsze wyniki <1, 1'000'000>", 100, [](const int& value) { return 0 < value && value <= 1'000'000; }),
                            api::var<int>("Ziarno", "Ustalona wartość inicjalizująca generator losowy w celu powtarzalności wyników (random seed). Ustaw 0 dla losowego ziarna", 0, [](const int& value) { return true; }),
                            api::var<int>("Opóźnienie", "Opóźnienie pomiędzy kolejnymi iteracjami (w milisekundach <0-3000>)", 10, [](const int& value) { return 0 <= value && value <= 3000; }));

    prepareSimulationThread();
}

SimpleController::~SimpleController()
{
    quitSimulationThread();
}

QUrl SimpleController::uiSource() const
{
    return QUrl("qrc:/qt/qml/simulit/gui/controllers/SimpleController.qml");
}

api::Variables SimpleController::properties()
{
    return m_properties;
}

ControllerState::State SimpleController::state() const
{
    return m_state;
}

void SimpleController::transitionTo(ControllerState::State state)
{
    m_state = state;
    m_isBusy = false;
    emit stateChanged(m_state);
}

void SimpleController::prepareSimulationThread()
{
    Q_ASSERT(m_simulationThread == nullptr);
    m_simulationThread = new QThread(this);
    auto simulation = dynamic_cast<api::SimpleSimulation*>(m_plugin->create());
    bindSignals(simulation);
    simulation->moveToThread(m_simulationThread);
    m_simulationThread->start();
}

void SimpleController::quitSimulationThread()
{
    if (m_simulationThread)
    {
        m_simulationThread->quit();
        m_simulationThread->wait();
        delete m_simulationThread;
        m_simulationThread = nullptr;
    }
}

void SimpleController::bindSignals(api::SimpleSimulation* simulation)
{
    // setup simulation
    QObject::connect(this, &controllers::SimpleController::setupSimulation,
                     simulation, &api::SimpleSimulation::_setup);

    // simulation declares ready to run
    QObject::connect(simulation, &api::SimpleSimulation::_setupFinished,
                     this, &controllers::SimpleController::onSimulationReadyToRun);

    // start simulation
    QObject::connect(this, &controllers::SimpleController::runSimulation,
                     simulation, &api::SimpleSimulation::_run);

    // update simulation statistics if library calls
    QObject::connect(simulation, &api::SimpleSimulation::progress,
                     this, &controllers::SimpleController::onSimulationUpdateProgress);

    // simulation finished the run
    QObject::connect(simulation, &api::SimpleSimulation::_runFinished,
                     this, &controllers::SimpleController::onSimulationRunFinished);

    // teardown simulations
    QObject::connect(this, &controllers::SimpleController::teardownSimulation,
                     simulation, &api::SimpleSimulation::_teardown);

    // simulation finished last run and teardown actions
    QObject::connect(simulation, &api::SimpleSimulation::_teardownFinished,
                     this, &controllers::SimpleController::simulationStop);

    // destroy object on thread end
    QObject::connect(m_simulationThread, &QThread::destroyed,
                     simulation, &QObject::deleteLater);

    // display error on screen if reported by the simulation
    QObject::connect(simulation, &api::SimpleSimulation::error,
                     this, &controllers::SimpleController::onSimulationError);
}

void SimpleController::onSimulationUpdateProgress(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
}

void SimpleController::onSimulationRunFinished(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
    nextRun();
}

void SimpleController::onSimulationError(const QString& message)
{
    simulationStop();
    emit error(message);
}

void SimpleController::simulationStart()
{
    auto propertyMap = api::VariableMap(m_properties);
    int iterations = propertyMap.ref<int>("Liczba przebiegów");
    int seed = propertyMap.ref<int>("Ziarno");
    int delayBetweenRuns = propertyMap.ref<int>("Opóźnienie");

    auto params = SimulationControlParams{};
    params.numberGenerator = std::unique_ptr<api::NumberGenerator>(tools::NumberGeneratorFactory().create(seed));
    params.currentIteration = 0;
    params.iterations = iterations;
    params.minDelayBetweenRuns = delayBetweenRuns;
    params.lastRunTimestamp = std::chrono::high_resolution_clock::now();
    std::swap(m_controlParams, params);

    emit setupSimulation(m_plugin->properties(), m_plugin->statistics());
}

void SimpleController::onSimulationReadyToRun(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
    transitionTo(ControllerState::Running);
    nextRun();
}

void SimpleController::nextRun()
{
    if (m_state == ControllerState::Paused)
    {
        return;
    }

    if (m_controlParams.currentIteration < m_controlParams.iterations)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(
                         now - m_controlParams.lastRunTimestamp).count();
        if (delay < m_controlParams.minDelayBetweenRuns)
        {
            auto diff = m_controlParams.minDelayBetweenRuns - delay;
            QTimer::singleShot(diff, this, &SimpleController::nextRun);
            return;
        }

        ++m_controlParams.currentIteration;
        m_controlParams.lastRunTimestamp = std::chrono::high_resolution_clock::now();
        emit runSimulation(m_controlParams.numberGenerator.get());
    }
    else
    {
        emit teardownSimulation();
    }
}

void SimpleController::simulationStop()
{
    quitSimulationThread();
    transitionTo(ControllerState::Stopped);
}

void SimpleController::simulationRestart()
{
    prepareSimulationThread();
    transitionTo(ControllerState::Ready);
}

bool SimpleController::isSimulationExists() const
{
    return m_simulationThread && m_simulationThread->isRunning();
}

void SimpleController::start()
{
    if (m_isBusy)
        return;
    m_isBusy = true;
    if (!isSimulationExists())
        simulationStop();

    if (m_state == ControllerState::Ready)
    {
        simulationStart();
    }
    else if (m_state == ControllerState::Paused)
    {
        transitionTo(ControllerState::Running);
        nextRun();
    }
}

void SimpleController::pause()
{
    if (m_isBusy)
        return;
    m_isBusy = true;
    if (!isSimulationExists())
        simulationStop();

    if (m_state == ControllerState::Running)
    {
        transitionTo(ControllerState::Paused);
    }
}

void SimpleController::stop()
{
    if (m_isBusy)
        return;
    m_isBusy = true;
    if (!isSimulationExists())
        simulationStop();

    if (m_state == ControllerState::Running || m_state == ControllerState::Paused)
    {
        simulationStop();
    }
}

void SimpleController::restart()
{
    if (m_isBusy)
        return;
    m_isBusy = true;

    simulationRestart();
}

}  // namespace controllers
