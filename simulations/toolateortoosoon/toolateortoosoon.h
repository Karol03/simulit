#pragma once

#include <QPoint>
#include "simulation.hpp"


// --- Animation configuration ---
// Handle variables useful for displaying animations
struct AnimationConfiguration
{
    AnimationConfiguration(double hMargins = 0.12L,
                           double vMargins = 0.12L,
                           int height = 600,
                           int width = 1066, /* image scale 16:9, same as imageFrame form ImageProvider, see AnimatedController.qml */
                           int horizontalScales = 5,
                           int verticalScales = 5,
                           int dividerLength = 12)
        : m_hMargins{hMargins}
        , m_vMargins{vMargins}
        , m_height{height}
        , m_width{width}
        , m_dividerHalfLength{dividerLength / 2}
        , m_calculatedHorizontalScales(horizontalScales)
        , m_calculatedVerticalScales(verticalScales)
    {
        calculate();
    }

    double hMargins() const { return m_hMargins; }
    void setHMargins(double value) { m_hMargins = value; calculate(); }

    double vMargins() const { return m_vMargins; }
    void setVMargins(double value) { m_vMargins = value; calculate(); }

    int height() const { return m_height; }
    void setHeight(int value) { m_height = value; calculate(); }

    int width() const { return m_width; }
    void setWidth(int value) { m_width = value; calculate(); }

    int horizontalScalesNumber() const { return m_calculatedHorizontalScales.size(); }
    void setHorizontalScalesNumber(int value) { m_calculatedHorizontalScales.resize(value); calculate(); }

    int verticalScalesNumber() const { return m_calculatedVerticalScales.size(); }
    void setVerticalScalesNumber(int value) { m_calculatedVerticalScales.resize(value); ; calculate(); }

    int dividerLength() const { return m_dividerHalfLength * 2; }
    void setDividerLength(int value) { m_dividerHalfLength = value / 2; calculate(); }

    const QPoint& axisYMaxPoint() const { return m_calculatedAxisYMaxPoint; }
    const QPoint& axisCenter() const { return m_calculatedAxisCenter; }
    const QPoint& axisXMaxPoint() const { return m_calculatedAxisXMaxPoint; }
    const QList<QLine>& horizontalScales() const { return m_calculatedHorizontalScales; }
    const QList<QLine>& verticalScales() const { return m_calculatedVerticalScales; }
    QPoint horizontalLabelPosition() const
    {
        const QPoint offset = {-46, 36};
        return (m_calculatedAxisCenter + m_calculatedAxisXMaxPoint) / 2 + offset;
    }
    QPoint verticalLabelPosition() const
    {
        const QPoint offset = {-59, 55};
        return (m_calculatedAxisCenter + m_calculatedAxisYMaxPoint) / 2 + offset;
    }
    QPoint legendPosition() const
    {
        const QPoint offset = {32, -(legendSize().height() / 2)};
        const QPoint maxXcenterY = {m_calculatedAxisXMaxPoint.x(),
                                    m_calculatedAxisYMaxPoint.y() + (m_calculatedAxisCenter.y() - m_calculatedAxisYMaxPoint.y()) / 2};
        return maxXcenterY + offset;
    }
    QSize legendSize() const
    {
        double axisHeight = (m_calculatedAxisCenter.y() - m_calculatedAxisYMaxPoint.y());
        return {110, static_cast<int>(axisHeight * 0.8L)};
    }
    QList<QPoint> horizontalScalesPositions() const
    {
        QList<QPoint> result(m_calculatedHorizontalScales.size() + 1);
        const QPoint offset = {-22, 28};
        result[0] = m_calculatedAxisCenter + offset;
        std::transform(m_calculatedHorizontalScales.begin(), m_calculatedHorizontalScales.end(),
                       ++result.begin(), [&offset](auto& value) { return QPoint{value.x1() + offset.x(), value.y1() + offset.y()}; });
        return result;
    }
    QList<QPoint> verticalScalesPositions() const
    {
        QList<QPoint> result(m_calculatedVerticalScales.size() + 1);
        const QPoint offset = {-46, 4};
        result[0] = m_calculatedAxisCenter + offset;
        std::transform(m_calculatedVerticalScales.begin(), m_calculatedVerticalScales.end(),
                       ++result.begin(), [&offset](auto& value) { return QPoint{value.x1() + offset.x(), value.y1() + offset.y()}; });
        return result;
    }

private:
    void calculate()
    {
        const int legendOffset = legendSize().width();
        const int leftMargin = static_cast<int>(m_width * m_hMargins);
        const int rightMargin = leftMargin + legendOffset;
        const int topMargin = static_cast<int>(m_height * m_vMargins);
        const int bottomMargin = topMargin;
        m_calculatedAxisYMaxPoint = {leftMargin, topMargin};
        m_calculatedAxisCenter = {leftMargin, m_height - bottomMargin};
        m_calculatedAxisXMaxPoint = {m_width - rightMargin, m_height - bottomMargin};
        calculateHorizontalScales();
        calculateVerticalScales();
    }

    void calculateHorizontalScales()
    {
        QPoint step, from, length;
        double totalLength = m_calculatedAxisXMaxPoint.x() - m_calculatedAxisCenter.x();
        int stepLength = totalLength / static_cast<double>(m_calculatedHorizontalScales.size());
        step = QPoint{stepLength, 0};
        length = QPoint{0, 2*m_dividerHalfLength};
        from = QPoint{m_calculatedAxisCenter.x(), m_calculatedAxisCenter.y() - m_dividerHalfLength};
        std::generate(m_calculatedHorizontalScales.begin(), m_calculatedHorizontalScales.end(), [&step, &from, &length]() {
            from += step;
            return QLine(from, from + length);
        });
    }

    void calculateVerticalScales()
    {
        QPoint step, from, length;
        double totalLength = m_calculatedAxisCenter.y() - m_calculatedAxisYMaxPoint.y();
        int stepLength = totalLength / static_cast<double>(m_calculatedVerticalScales.size());
        step = QPoint{0, -stepLength};
        length = QPoint{2*m_dividerHalfLength, 0};
        from = QPoint{m_calculatedAxisCenter.x() - m_dividerHalfLength, m_calculatedAxisCenter.y()};
        std::generate(m_calculatedVerticalScales.begin(), m_calculatedVerticalScales.end(), [&step, &from, &length]() {
            from += step;
            return QLine(from, from + length);
        });
    }

private:
    double m_hMargins;
    double m_vMargins;
    int m_height;
    int m_width;
    int m_dividerHalfLength;

    QPoint m_calculatedAxisYMaxPoint;
    QPoint m_calculatedAxisCenter;
    QPoint m_calculatedAxisXMaxPoint;
    QList<QLine> m_calculatedHorizontalScales;
    QList<QLine> m_calculatedVerticalScales;
};


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
    AnimationConfiguration animationConfiguration;

    // --- Variables from properties ---
    int busArrivalFrom = 0;
    int busArrivalTo = 0;
    int boyArrivalFrom = 0;
    int boyArrivalTo = 0;
    bool animate = false;

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
    int currentRow = 0;
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
