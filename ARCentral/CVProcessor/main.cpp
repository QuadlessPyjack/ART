#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "qstreamintegratedview.h"
#include "frameprocessor.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<QStreamIntegratedView>("com.test.classes", 1, 0, "StreamIntegratedView");
    qmlRegisterType<FrameProcessor>("com.test.classes", 1, 0, "FrameProcessor");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
