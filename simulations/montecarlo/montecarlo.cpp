#include "montecarlo.h"

#include <QPainter>
#include <cmath>


// Helper function to map coordinates
namespace
{
auto map(double value, double from, double to, double plotFrom, double plotTo)
{
    return static_cast<int>((value - from) * (plotTo - plotFrom) / (to - from) + plotFrom);
}
}

MonteCarloSimulationDLL::MonteCarloSimulationDLL(QObject* parent)
    : QObject(parent)
{
    m_properties = api::var("Symulacja", this,
                            api::var<bool>("Animowanie", "Włącza rysowanie punktów na wykresie\npunktów wewnątrz i na zewnątrz koła", false));

    m_statistics = api::var(this,
                            api::var<int>("Próby", "Całkowita liczba wylosowanych punktów", 0),
                            api::var<int>("Wewnątrz koła", "Liczba punktów wewnątrz ćwiartki koła", 0),
                            api::var<double>("Oszacowanie π", "Aktualne oszacowanie liczby π", 0.0),
                            api::var<double>("Błąd", "Błąd bezwzględny względem prawdziwej wartości π", 0.0));
}

QString MonteCarloSimulationDLL::name() const
{
    return "Obliczanie π Monte Carlo";
}

QString MonteCarloSimulationDLL::description() const
{
    return "Symulacja Monte Carlo służąca do oszacowania wartości liczby π.\n\n"
           "Metoda polega na losowaniu punktów w kwadracie o boku 1 i sprawdzaniu, "
           "czy punkt znajduje się wewnątrz ćwiartki koła o promieniu 1 wpisanego w ten kwadrat.\n\n"
           "Stosunek liczby punktów wewnątrz koła do wszystkich punktów jest proporcjonalny "
           "do stosunku pola ćwiartki koła (π/4) do pola kwadratu (1).\n\n"
           "Wzór: π ≈ 4 × (liczba punktów w kole / wszystkie punkty)\n\n"
           "Im więcej punktów wylosujemy, tym dokładniejsze oszacowanie π otrzymamy.";
}

api::ISimulation* MonteCarloSimulationDLL::create() const
{
    return new MonteCarloSimulation();
}

api::Variables MonteCarloSimulationDLL::properties() const
{
    return m_properties;
}

api::Variables MonteCarloSimulationDLL::statistics() const
{
    return m_statistics;
}

void MonteCarloSimulation::setup(api::VariableWatchList properties)
{
    // Get properties
    animate = properties.get<bool>("Animowanie");

    // Get statistics pointers
    trials = &stats.ref<int>("Próby");
    hits = &stats.ref<int>("Wewnątrz koła");
    piEstimate = &stats.ref<double>("Oszacowanie π");
    error = &stats.ref<double>("Błąd");

    // Setup animation if enabled
    if (animate)
    {
        animationConfiguration = montecarlo::AnimationConfiguration{};

        image = QImage(animationConfiguration.width(),
                       animationConfiguration.height(),
                       QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);

        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);

        p.setPen(Qt::white);

        QRect squareRect(animationConfiguration.position(),
                         animationConfiguration.position() + animationConfiguration.size());
        p.drawRect(squareRect);

        const int side = squareRect.width();
        QRect circleRect(squareRect.left() - side,
                         squareRect.bottom() - side,
                         2 * side, 2 * side);
        p.drawArc(circleRect, 0 * 16, 90 * 16);

        // Labels
        p.drawText(squareRect.right() - 40, squareRect.bottom() + 15, "x");
        p.drawText(squareRect.left() - 10, squareRect.top() + 10, "y");

        // Legend
        const int radius = 4;
        QPoint legendPos(squareRect.right() - 120, squareRect.top() + 20);

        p.setPen(Qt::NoPen);
        p.setBrush(Qt::darkGreen);
        p.drawEllipse(legendPos, radius, radius);
        p.setPen(Qt::white);
        p.drawText(legendPos + QPoint(10, 4), "W kole");

        legendPos += QPoint(0, 20);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::red);
        p.drawEllipse(legendPos, radius, radius);
        p.setPen(Qt::white);
        p.drawText(legendPos + QPoint(10, 4), "Poza kołem");
    }
}

void MonteCarloSimulation::run(api::NumberGenerator& generator)
{
    const double x = generator.real(0.0L, 1.0L);
    const double y = generator.real(0.0L, 1.0L);
    const double dist2 = x * x + y * y;

    ++(*trials);
    if (dist2 <= 1.0)
    {
        ++(*hits);
    }

    // Pi ≈ 4 * hits / trials
    *piEstimate = 4.0 * static_cast<double>(*hits) / static_cast<double>(*trials);

    const double piTrue = 4.0 * std::atan(1.0); // π = 4 * arctan(1)
    *error = std::fabs(*piEstimate - piTrue);

    if (animate)
    {
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing, false);

        const auto squarePosition = animationConfiguration.position();
        const auto squarePositionEnd = animationConfiguration.position() + animationConfiguration.size();
        const int px = map(x, 0.0L, 1.0L, squarePosition.x(), squarePositionEnd.x());
        const int py = map(y, 0.0L, 1.0L, squarePositionEnd.y(), squarePosition.y());
        if (dist2 <= 1.0)
            p.setPen(Qt::darkGreen);
        else
            p.setPen(Qt::red);

        p.drawPoint(px, py);
    }
}

void MonteCarloSimulation::teardown()
{
    /* nothing to do on clear in this simulation */
}
