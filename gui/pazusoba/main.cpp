#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "bridge.h"

int main(int argc, char *argv[])
{
    // Scale to large screen and some setups
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Material");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Register types to use in QML
    qRegisterMetaType<QRoute *>("QRoute*");
    qmlRegisterType<QBridge>("org.github.henryquan.bridge", 1, 0, "QBridge");

    // Actually load the qml
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
