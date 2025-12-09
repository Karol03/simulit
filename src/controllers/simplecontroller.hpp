#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QThread>
#include <chrono>

#include "icontroller.hpp"
#include "providers/statistics.hpp"
#include "ControllerState.hpp"


namespace api
{
class ISimulationDLL;
class SimpleSimulation;
class NumberGenerator;
}  // namespace api


namespace controllers
{

class SimpleController : public IController
{
    Q_OBJECT

    Q_PROPERTY(ControllerState::State state READ state NOTIFY stateChanged)

    struct SimulationControlParams
    {
        std::unique_ptr<api::NumberGenerator> numberGenerator;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastRunTimestamp;
        int minDelayBetweenRuns;
        int currentIteration;
        int iterations;
    };

public:
    SimpleController(api::ISimulationDLL* plugin,
                     providers::Statistics& statisticsToUpdate,
                     QObject* parent = nullptr);
    ~SimpleController();

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void restart();

    QUrl uiSource() const override;
    api::Variables properties() override;

    ControllerState::State state() const;

signals:
    void stateChanged(ControllerState::State state); // clazy:exclude=fully-qualified-moc-types
    void error(const QString& message);

    void setupSimulation(api::Variables properties, api::Variables statistics); // clazy:exclude=fully-qualified-moc-types
    void runSimulation(api::NumberGenerator* generator);
    void teardownSimulation();

private slots:
    void onSimulationReadyToRun(const api::VariableMapSnapshot& update);
    void onSimulationUpdateProgress(const api::VariableMapSnapshot& update);
    void onSimulationRunFinished(const api::VariableMapSnapshot& update);
    void onSimulationError(const QString& message);

private:
    void prepareSimulationThread();
    void quitSimulationThread();
    bool isSimulationExists() const;
    void nextRun();

    void bindSignals(api::SimpleSimulation* simulation);
    void simulationStart();

    void simulationStop();
    void simulationRestart();

    void transitionTo(ControllerState::State state);

private:
    api::ISimulationDLL* m_plugin;
    providers::Statistics& m_statistics;
    api::Variables m_properties;
    QThread* m_simulationThread;
    ControllerState::State m_state;
    bool m_isBusy;
    SimulationControlParams m_controlParams;
};

}  // namespace controllers
