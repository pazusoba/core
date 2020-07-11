#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include "../../lib/solver.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    // Call my solver
    using namespace std;
    string filePath = "../../assets/sample_board_65.txt";
    int minEraseCondition = 3;
    int maxStep = 20;
    int maxSize = 1000;
    auto soba = new PSolver(filePath, minEraseCondition, maxStep, maxSize);
    // This line will freeze the UI
//    cout << soba -> solve() << endl;
    delete soba;

    return app.exec();
}
