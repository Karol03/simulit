#include "toolateortoosoon.h"

#include <QRegularExpression>


TooLateOrTooSoonSimulationDLL::TooLateOrTooSoonSimulationDLL(QObject* parent)
    : QObject(parent)
{
    auto matchTimeFormat = [](const QString& value) {
        // regexp to check time format hh:mm | hh:mm:ss
        QRegularExpression timeFormat(R"(^(\d|[01]\d|2[0-3]):[0-5]\d(?::[0-5]\d)?$)");
        return timeFormat.match(value).hasMatch();
    };

    m_properties = api::var("Symulacja", this,
        api::var("Autobus",
            api::var<QString>("Najwcześniej", "Najwcześniejsza godzina przyjazdu autobusu w formacie hh:mm lub hh:mm:ss", "", matchTimeFormat),
            api::var<QString>("Najpóźniej", "Najpóźniejsza godzina przyjazdu autobusu w formacie hh:mm lub hh:mm:ss", "", matchTimeFormat)),
        api::var("Chłopiec",
            api::var<QString>("Najwcześniej", "Najwcześniejsza godzina przyjazdu chłopca na przystanek w formacie hh:mm lub hh:mm:ss", "", matchTimeFormat),
            api::var<QString>("Najpóźniej", "Najpóźniejsza godzina przyjazdu chłopca na przystanek w formacie hh:mm lub hh:mm:ss", "", matchTimeFormat)));
    m_statistics = api::var(this,
        api::var<int>("Próby", "Liczba prób", 0),
        api::var<int>("Na czas", "Ile razy chłopiec zdążył na autobus", 0),
        api::var<int>("Najdłuższa seria na czas", "Najdłuższa seria dni, gdy chłopiec był na czas", 0),
        api::var<QString>("Średni czas czekania", "Średni czas oczekiwania, gdy chłopiec się nie spóźnił"),
        api::var<int>("Spóźnienia", "Ile razy chłopiec spóźnił się na autobus", 0),
        api::var<QString>("Średni spóźnienie", "Średni czas spóźnienia, gdy chłopiec przyjechał zbyt późno"),
        api::var<int>("Najdłuższa seria spóźnień", "Najdłuższa seria dni, gdy chłopiec się spóźnił", 0));
}

QString TooLateOrTooSoonSimulationDLL::name() const
{
    return "Za wcześnie czy za późno?";
}

QString TooLateOrTooSoonSimulationDLL::description() const
{
    return "Chłopiec regularnie spóźnia się do szkoły, choć stara się być punktualny.\n"
           "Każdego ranka ojciec wiezie go na przystanek, gdzie autobus ma przyjechać "
           "około 8:00. W rzeczywistości jednak autobus może pojawić się w dowolnym "
           "momencie między 7:58 a 8:02 i od razu odjeżdża. Ojciec i syn również "
           "nie są w stanie przewidzieć dokładnego czasu dotarcia na przystanek —"
           " w zależności od warunków drogowych dojeżdżają tam między 7:55 a 8:01.\n\n"
           "Przez tę różnicę w nieprzewidywalnych przedziałach czasowych chłopiec "
           "często przyjeżdża za późno i autobus odjeżdża mu sprzed nosa, co prowadzi "
           "do spóźnień do szkoły mimo szczerych chęci.\n\n"
           "Symulacja prezentuje przedstawiony powyżej problem.";
}

api::ISimulation* TooLateOrTooSoonSimulationDLL::create() const
{
    return new TooLateOrTooSoonSimulation();
}

api::Variables TooLateOrTooSoonSimulationDLL::properties() const
{
    return m_properties;
}

api::Variables TooLateOrTooSoonSimulationDLL::statistics() const
{
    return m_statistics;
}

void TooLateOrTooSoonSimulation::setup(api::VariableWatchList properties)
{
    // Converts time in hh:mm or hh:mm:ss format to int
    const auto convertTime = [](const QString& value)
    {
        const QStringList parts = value.split(':');
        int hours   = parts[0].toInt();
        int minutes = parts[1].toInt();
        int seconds = (parts.size() == 3) ? parts[2].toInt() : 0;
        return hours * 3600 + minutes * 60 + seconds;
    };

    // Get properties as QString
    auto busArrivalFromStr = properties.get<QString>("Autobus::Najwcześniej");
    auto busArrivalToStr = properties.get<QString>("Autobus::Najpóźniej");
    auto boyArrivalFromStr = properties.get<QString>("Chłopiec::Najwcześniej");
    auto boyArrivalToStr = properties.get<QString>("Chłopiec::Najpóźniej");

    // Converted properties from QString "hh:mm" or "hh:mm:ss" to seconds since midnight
    busArrivalFrom = convertTime(busArrivalFromStr);
    busArrivalTo = convertTime(busArrivalToStr);
    boyArrivalFrom = convertTime(boyArrivalFromStr);
    boyArrivalTo = convertTime(boyArrivalToStr);

    // Check whether the time specified in the parameter 'to' follows the parameter 'from'
    // Report error if not
    if (busArrivalFrom > busArrivalTo)
        emit error(QString("Autobus wartość najwcześniej następuje po najpóźniej\n%1 > %2\nNieprawidłowa kolejność").arg(busArrivalFromStr, busArrivalToStr));

    if (boyArrivalFrom > boyArrivalTo)
        emit error(QString("Autobus wartość najwcześniej następuje po najpóźniej\n%1 > %2\nNieprawidłowa kolejność").arg(boyArrivalFromStr, boyArrivalToStr));

    // Get statistics pointers to update variables in run
    trials = &stats.ref<int>("Próby");
    onTime = &stats.ref<int>("Na czas");
    late = &stats.ref<int>("Spóźnienia");
    averageWaitingTime = &stats.ref<QString>("Średni czas czekania");
    averageDelayTime = &stats.ref<QString>("Średni spóźnienie");
    longestOnTimeSeries = &stats.ref<int>("Najdłuższa seria na czas");
    longestLateSeries = &stats.ref<int>("Najdłuższa seria spóźnień");
}

void TooLateOrTooSoonSimulation::run(api::NumberGenerator& generator)
{
    static const auto convertToTime = [](const auto& value)
    {
        const auto seconds = value % 60;
        const auto minutesOverall = (value - seconds) / 60;
        const auto minutes = minutesOverall % 60;
        const auto hours = minutesOverall / 60;
        return QString::asprintf("%02d:%02d:%02d", hours, minutes, seconds);
    };

    const auto busArrivalTime = generator(busArrivalFrom, busArrivalTo);
    const auto boyArrivalTime = generator(boyArrivalFrom, boyArrivalTo);

    ++(*trials);
    if (boyArrivalTime <= busArrivalTime)
    {
        // Boy is on time
        ++(*onTime);
        const int64_t waitingTime = busArrivalTime - boyArrivalTime;
        totalWaitingTimeInt += waitingTime;
        *averageWaitingTime = convertToTime(totalWaitingTimeInt / static_cast<int64_t>(*trials));
        series = std::max(series + 1, 1);
        *longestOnTimeSeries = std::max(*longestOnTimeSeries, series);
    }
    else
    {
        // Boy is late
        ++(*late);
        const int64_t delayTime = boyArrivalTime - busArrivalTime;
        totalDelayTimeInt += delayTime;
        *averageDelayTime = convertToTime(totalDelayTimeInt / static_cast<int64_t>(*trials));
        series = std::min(series - 1, -1);
        *longestLateSeries = std::max(*longestLateSeries, -series);
    }
}

void TooLateOrTooSoonSimulation::teardown()
{
    /* nothing to do on clear in this simulation */
}
