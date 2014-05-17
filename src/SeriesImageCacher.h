#ifndef SERIESIMAGECACHER_H
#define SERIESIMAGECACHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQmlHelpers>

class SeriesCommon {
public:
    static   SeriesCommon * getInstance ();
    QNetworkAccessManager * getNAM      () const;

    QString localFileFromRemoteUrl      (QString remoteUrl);

private:
    QString                 m_path;
    QNetworkAccessManager * m_nam;

    static   SeriesCommon * s_instance;
    explicit SeriesCommon   ();
};

class SeriesImageCacher : public QObject {
    Q_OBJECT
    QML_WRITABLE_PROPERTY (QString, remoteSource)
    QML_READONLY_PROPERTY (QString, localSource)

public:
    explicit SeriesImageCacher (QObject * parent = NULL);

private slots:
    void onRequestFinished     ();
    void onRemoteSourceChanged ();
};

#endif // SERIESIMAGECACHER_H
