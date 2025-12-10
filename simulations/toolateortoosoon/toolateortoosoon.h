#pragma once

#include "simulation.hpp"


class TooLateOrTooSoonSimulation : public api::AnimatedSimulation
{
    Q_OBJECT

public:
    using api::AnimatedSimulation::AnimatedSimulation;

public:
    void setup(api::VariableWatchList properties) override;
    void run(api::NumberGenerator& generator) override;
    void teardown() override;

private:
    // --- Variables from properties ---
    int busArrivalFrom = 0;
    int busArrivalTo = 0;
    int boyArrivalFrom = 0;
    int boyArrivalTo = 0;

    // --- Variables for statistics ---
    int* trials = nullptr;
    int* onTime = nullptr;
    int* late = nullptr;
    QString* averageWaitingTime = nullptr;
    QString* averageDelayTime = nullptr;
    int* longestOnTimeSeries = nullptr;
    int* longestLateSeries = nullptr;
    double* percentOfDelays = nullptr;

    // --- Suuport variables for statistics ---
    // not present in UI but required to calculate others
    int64_t totalWaitingTimeInt = 0;
    int64_t totalDelayTimeInt = 0;
    int series = 0;
};


class TooLateOrTooSoonSimulationDLL : public QObject, public api::ISimulationDLL
{
    Q_OBJECT
    Q_INTERFACES(api::ISimulationDLL)
    Q_PLUGIN_METADATA(IID ISimulationDLL_iid)

public:
    explicit TooLateOrTooSoonSimulationDLL(QObject* parent = nullptr);

    QString name() const override;
    QString description() const override;
    api::ISimulation* create() const override;
    api::Variables properties() const override;
    api::Variables statistics() const override;

private:
    api::Variables m_properties;
    api::Variables m_statistics;
};
