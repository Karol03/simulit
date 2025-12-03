#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "simulationprovider/simulationprovider.hpp"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    simulationprovider::SimulationProvider simulationProvider;

    engine.rootContext()->setContextProperty("simulationProvider", &simulationProvider);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("simulit", "Main");

    // After the engine loads, to see all error dialogs and loaded simulations
    simulationProvider.loadSimulations();

    return app.exec();
}
