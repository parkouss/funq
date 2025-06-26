#include <QQuickView>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView * view = new QQuickView();
    view->setSource(QUrl("qrc:///qml/children.qml"));
    view->show();
    return app.exec();
}
