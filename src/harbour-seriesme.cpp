#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include <QAbstractListModel>
#include <QNetworkProxy>
#include <QQuickView>
#include <QStringList>
#include <sailfishapp.h>
#include <QSettings>
#include <qqml.h>

#include "SeriesEngine.h"
#include "SeriesItem.h"

#define QML_MODULE "harbour.seriesme.myPrivateImports", 1, 0

int main (int argc, char * argv []) {
    QSettings::setDefaultFormat (QSettings::IniFormat);
    QGuiApplication::setApplicationName ("harbour-seriesme");
    qmlRegisterUncreatableType<QAbstractItemModel>    (QML_MODULE, "AbstractItemModel",  "!!!");
    qmlRegisterUncreatableType<QAbstractListModel>    (QML_MODULE, "AbstractListModel",  "!!!");
    qmlRegisterUncreatableType<QQmlObjectListModel>   (QML_MODULE, "ObjectListModel",    "!!!");
    qmlRegisterType<SeriesEngine>                     (QML_MODULE, "SeriesEngine");
    qmlRegisterType<SeriesItem>                       (QML_MODULE, "SeriesItem");
    QGuiApplication * app = SailfishApp::application (argc, argv);
    if (!qgetenv ("HTTP_PROXY").isEmpty ()) {
        QString proxyStr = QString::fromLocal8Bit (qgetenv ("HTTP_PROXY")).toLower ().remove ("http://");
        QNetworkProxy::setApplicationProxy (QNetworkProxy (QNetworkProxy::HttpProxy, proxyStr.split (':').first (), proxyStr.split (':').last ().toInt ()));
    }
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl ("qrc:/qml/harbour-seriesme.qml"));
    view->show ();
    return app->exec ();
}
