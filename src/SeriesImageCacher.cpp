
#include "SeriesImageCacher.h"

#include <QCryptographicHash>
#include <QStandardPaths>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QUrl>

#define EMPTY_QSTR QStringLiteral ("")

SeriesCommon * SeriesCommon::s_instance = NULL;
SeriesCommon * SeriesCommon::getInstance () {
    if (!s_instance) {
        s_instance = new SeriesCommon;
    }
    return s_instance;
}

SeriesCommon::SeriesCommon () {
    m_nam  = new QNetworkAccessManager;
    m_path = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
}

QString SeriesCommon::localFileFromRemoteUrl (QString remoteUrl) {
    return QString ("%1/images/%2.jpeg").arg (m_path).arg (QString::fromLocal8Bit (QCryptographicHash::hash (remoteUrl.toLocal8Bit (), QCryptographicHash::Md5).toHex ()));
}

QNetworkAccessManager * SeriesCommon::getNAM () const {
    return m_nam;
}

SeriesImageCacher::SeriesImageCacher (QObject * parent) : QObject (parent) {
    m_localSource  = EMPTY_QSTR;
    m_remoteSource = EMPTY_QSTR;
    connect (this,  &SeriesImageCacher::remoteSourceChanged,
             this,  &SeriesImageCacher::onRemoteSourceChanged);
}

void SeriesImageCacher::onRemoteSourceChanged () {
    if (!m_remoteSource.isEmpty ()) {
        QString localSource = SeriesCommon::getInstance ()->localFileFromRemoteUrl (m_remoteSource);
        QFile localFile (localSource);
        if (localFile.exists ()) {
            update_localSource (QUrl::fromLocalFile (localSource).toString ());
        }
        else {
            QNetworkReply * reply = SeriesCommon::getInstance ()->getNAM ()->get (QNetworkRequest (QUrl (m_remoteSource)));
            reply->setProperty ("localSource", localSource);
            connect (reply, &QNetworkReply::finished, this,  &SeriesImageCacher::onRequestFinished);
            update_localSource (EMPTY_QSTR);
        }
    }
    else {
        update_localSource (EMPTY_QSTR);
    }
}

void SeriesImageCacher::onRequestFinished () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ());
    if (reply) {
        if (reply->error () == QNetworkReply::NoError) {
            QByteArray data = reply->readAll ();
            if (!data.isEmpty ()) {
                QString localSource  = reply->property ("localSource").toString ();
                QFile localFile (localSource);
                if (localFile.open  (QIODevice::WriteOnly | QIODevice::Truncate)) {
                    localFile.write (data);
                    localFile.flush ();
                    localFile.close ();
                }
                update_localSource (QUrl::fromLocalFile (localSource).toString ());
            }
            else {
                qWarning () << "Image data empty, not cached !";
                update_localSource (EMPTY_QSTR);
            }
        }
        else {
            qWarning () << "Image download error :" << reply->errorString ();
            update_localSource (EMPTY_QSTR);
        }
    }
}
