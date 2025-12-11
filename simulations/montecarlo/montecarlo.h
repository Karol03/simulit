#pragma once

#include <QPoint>
#include "simulation.hpp"


// --- Animation configuration ---
// Handle variables useful for displaying animations
namespace montecarlo
{
struct AnimationConfiguration
{
    AnimationConfiguration(double hMargins = 0.12L,
                           double vMargins = 0.12L,
                           int height = 600,
                           int width = 1066) /* image scale 16:9, same as imageFrame form ImageProvider, see AnimatedController.qml */
        : m_hMargins{hMargins}
        , m_vMargins{vMargins}
        , m_height{height}
        , m_width{width}
    {}

    double hMargins() const { return m_hMargins; }
    void setHMargins(double value) { m_hMargins = value; }

    double vMargins() const { return m_vMargins; }
    void setVMargins(double value) { m_vMargins = value; }

    int availableHeight() const { return static_cast<int>(double(m_height) * (1.0 - 2.0 * m_vMargins)); }
    int height() const { return m_height; }
    void setHeight(int value) { m_height = value; }

    int availableWidth() const { return static_cast<int>(double(m_width) * (1.0 - 2.0 * m_hMargins)); }
    int width() const { return m_width; }
    void setWidth(int value) { m_width = value; }

    QPoint size() const
    {
        const auto wallSize = std::min(availableHeight(), availableWidth());
        return {wallSize, wallSize};
    }

    QPoint position() const
    {
        const auto aWidth = availableWidth();
        const auto aHeight = availableHeight();
        if (aWidth > aHeight)
            return QPoint(m_width * m_hMargins + (aWidth - aHeight) / 2, m_height * m_vMargins);
        else
            return QPoint(m_width * m_hMargins, m_height * m_vMargins + (aHeight - aWidth) / 2);
    }

private:
    double m_hMargins;
    double m_vMargins;
    int m_height;
    int m_width;
};
}  // namespace montecarlo


class MonteCarloSimulation : public api::AnimatedSimulation
{
    Q_OBJECT

public:
    using api::AnimatedSimulation::AnimatedSimulation;

public:
    void setup(api::VariableWatchList properties) override;
    void run(api::NumberGenerator& generator) override;
    void teardown() override;

private:
    montecarlo::AnimationConfiguration animationConfiguration;

    // --- Variables from properties ---
    bool animate = false;

    // --- Variables for statistics ---
    int* trials = nullptr;
    int* hits = nullptr;
    double* piEstimate = nullptr;
    double* error = nullptr;

    // --- Support variables for statistics ---
    // not present in UI but required to calculate others
};


class MonteCarloSimulationDLL : public QObject, public api::ISimulationDLL
{
    Q_OBJECT
    Q_INTERFACES(api::ISimulationDLL)
    Q_PLUGIN_METADATA(IID ISimulationDLL_iid)

public:
    explicit MonteCarloSimulationDLL(QObject* parent = nullptr);

    QString name() const override;
    QString description() const override;
    api::ISimulation* create() const override;
    api::Variables properties() const override;
    api::Variables statistics() const override;

private:
    api::Variables m_properties;
    api::Variables m_statistics;
};
