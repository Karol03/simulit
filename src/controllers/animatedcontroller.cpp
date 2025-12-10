#include "animatedcontroller.hpp"

#include <QTimer>
#include "api/simulation.hpp"
#include "tools/numbergeneratorfactory.hpp"


namespace controllers
{

AnimatedController::AnimatedController(api::ISimulationDLL* plugin,
                                   QObject* parent)
    : IController(parent)
    , m_plugin{plugin}
    , m_statistics{plugin->statistics()}
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

AnimatedController::~AnimatedController()
{
    quitSimulationThread();
}

QUrl AnimatedController::uiSource() const
{
    return QUrl("qrc:/qt/qml/simulit/gui/controllers/AnimatedController.qml");
}

api::Variables AnimatedController::properties()
{
    return m_properties;
}

providers::IProvider* AnimatedController::statistics()
{
    return &m_statistics;
}

ControllerState::State AnimatedController::state() const
{
    return m_state;
}

void AnimatedController::transitionTo(ControllerState::State state)
{
    m_state = state;
    m_isBusy = false;
    emit stateChanged(m_state);
}

void AnimatedController::prepareSimulationThread()
{
    Q_ASSERT(m_simulationThread == nullptr);
    m_simulationThread = new QThread(this);
    auto simulation = dynamic_cast<api::AnimatedSimulation*>(m_plugin->create());
    bindSignals(simulation);
    simulation->moveToThread(m_simulationThread);
    m_simulationThread->start();
}

void AnimatedController::quitSimulationThread()
{
    if (m_simulationThread)
    {
        m_simulationThread->quit();
        m_simulationThread->wait();
        delete m_simulationThread;
        m_simulationThread = nullptr;
    }
}

void AnimatedController::bindSignals(api::AnimatedSimulation* simulation)
{
    // setup simulation
    QObject::connect(this, &controllers::AnimatedController::setupSimulation,
                     simulation, &api::AnimatedSimulation::_setup);

    // simulation declares ready to run
    QObject::connect(simulation, &api::AnimatedSimulation::_setupFinished,
                     this, &controllers::AnimatedController::onSimulationReadyToRun);

    // start simulation
    QObject::connect(this, &controllers::AnimatedController::runSimulation,
                     simulation, &api::AnimatedSimulation::_run);

    // update simulation statistics if library calls
    QObject::connect(simulation, &api::AnimatedSimulation::progress,
                     this, &controllers::AnimatedController::onSimulationUpdateProgress);

    // simulation finished the run
    QObject::connect(simulation, &api::AnimatedSimulation::_runFinished,
                     this, &controllers::AnimatedController::onSimulationRunFinished);

    // teardown simulations
    QObject::connect(this, &controllers::AnimatedController::teardownSimulation,
                     simulation, &api::AnimatedSimulation::_teardown);

    // simulation finished last run and teardown actions
    QObject::connect(simulation, &api::AnimatedSimulation::_teardownFinished,
                     this, &controllers::AnimatedController::simulationStop);

    // destroy object on thread end
    QObject::connect(m_simulationThread, &QThread::destroyed,
                     simulation, &QObject::deleteLater);

    // display error on screen if reported by the simulation
    QObject::connect(simulation, &api::AnimatedSimulation::error,
                     this, &controllers::AnimatedController::onSimulationError);
}

void AnimatedController::onSimulationUpdateProgress(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
}

void AnimatedController::onSimulationRunFinished(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
    nextRun();
}

void AnimatedController::onSimulationError(const QString& message)
{
    simulationStop();
    emit error(message);
}

void AnimatedController::simulationStart()
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

void AnimatedController::onSimulationReadyToRun(const api::VariableMapSnapshot& update)
{
    m_statistics.updateWatched(update);
    transitionTo(ControllerState::Running);
    nextRun();
}

void AnimatedController::nextRun()
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
            QTimer::singleShot(diff, this, &AnimatedController::nextRun);
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

void AnimatedController::simulationStop()
{
    quitSimulationThread();
    transitionTo(ControllerState::Stopped);
}

void AnimatedController::simulationRestart()
{
    prepareSimulationThread();
    transitionTo(ControllerState::Ready);
}

bool AnimatedController::isSimulationExists() const
{
    return m_simulationThread && m_simulationThread->isRunning();
}

void AnimatedController::start()
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

void AnimatedController::pause()
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

void AnimatedController::stop()
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

void AnimatedController::restart()
{
    if (m_isBusy)
        return;
    m_isBusy = true;

    simulationRestart();
}

}  // namespace controllers
