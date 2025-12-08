#include "simpleworker.hpp"

#include "adapters/property.hpp"


namespace providers::workerproperties
{

SimpleWorker::SimpleWorker(QObject* parent)
    : IProvider(parent)
{
    m_rootProperty = api::var("Przebieg", this,
                              api::var<int>("Liczba przebiegów", "Liczba powtórzeń symulacji, im większa, tym dokładniejsze wyniki <1, 10'000'000>", 1, [](const int& value) { return 0 < value && value <= 10'000'000; }),
                              api::var<int>("Ziarno", "Ustalona wartość inicjalizująca generator losowy w celu powtarzalności wyników (random seed). Ustaw 0 dla losowego ziarna", 0, [](const int& value) { return true; }),
                              api::var<int>("Opóźnienie", "Opóźnienie pomiędzy kolejnymi iteracjami (w milisekundach <0-3000>)", 20, [](const int& value) { return 0 <= value && value <= 3000; }));
    createAdapters();
}

QObjectList SimpleWorker::obtain()
{
    return m_adapters;
}

adapters::IAdapter* SimpleWorker::select(const QString&)
{
    // select any of SimpleWorker properties do nothing
    return nullptr;
}

void SimpleWorker::createAdapters()
{
    QObjectList variablesWithName;
    m_rootProperty->preorderTraversalSquash(variablesWithName,
                                            [](const auto& variable) { return !variable.name().isEmpty(); });
    for (auto& property : variablesWithName)
    {
        m_adapters.append(new adapters::Property(dynamic_cast<api::IVariable*>(property), this));
    }
}

void SimpleWorker::updateFromMap(const QVariantMap& update)
{
    /* properties updated only from GUI, ignore */
}

}  // namespace providers
