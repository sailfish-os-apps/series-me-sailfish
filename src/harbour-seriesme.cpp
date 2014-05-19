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
#include "SeriesSeason.h"
#include "SeriesEpisode.h"
#include "SeriesImageCacher.h"

#define QML_MODULE "harbour.seriesme.myPrivateImports", 1, 0

int main (int argc, char * argv []) {
    QSettings::setDefaultFormat (QSettings::IniFormat);
    QGuiApplication::setApplicationName ("harbour-seriesme");
    qmlRegisterUncreatableType<QAbstractItemModel>    (QML_MODULE, "AbstractItemModel",  "!!!");
    qmlRegisterUncreatableType<QAbstractListModel>    (QML_MODULE, "AbstractListModel",  "!!!");
    qmlRegisterUncreatableType<QQmlObjectListModel>   (QML_MODULE, "ObjectListModel",    "!!!");
    qmlRegisterType<SeriesImageCacher>                (QML_MODULE, "SeriesImageCacher");
    qmlRegisterType<SeriesItem>                       (QML_MODULE, "SeriesItem");
    qmlRegisterType<SeriesSeason>                     (QML_MODULE, "SeriesSeason");
    qmlRegisterType<SeriesEpisode>                    (QML_MODULE, "SeriesEpisode");
    qmlRegisterType<SeriesEngine>                     (QML_MODULE, "SeriesEngine");
    QGuiApplication * app = SailfishApp::application (argc, argv);
    if (!qgetenv ("HTTP_PROXY").isEmpty ()) {
        QString proxyStr = QString::fromLocal8Bit (qgetenv ("HTTP_PROXY")).toLower ().remove (QStringLiteral ("http://"));
        QNetworkProxy::setApplicationProxy (QNetworkProxy (QNetworkProxy::HttpProxy, proxyStr.split (':').first (), proxyStr.split (':').last ().toInt ()));
    }
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl (QStringLiteral ("qrc:/qml/harbour-seriesme.qml")));
    view->show ();
    return app->exec ();
}
