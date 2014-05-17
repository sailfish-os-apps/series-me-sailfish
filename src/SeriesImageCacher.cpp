
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
    m_localSource  = QStringLiteral ("");
    m_remoteSource = QStringLiteral ("");
    connect (this,  &SeriesImageCacher::remoteSourceChanged,
             this,  &SeriesImageCacher::onRemoteSourceChanged);
}

void SeriesImageCacher::onRemoteSourceChanged (QString remoteSource) {
    Q_UNUSED (remoteSource)
    if (!m_remoteSource.isEmpty ()) {
        QString localSource = SeriesCommon::getInstance ()->localFileFromRemoteUrl (m_remoteSource);
        qDebug () << "onRemoteSourceChanged :"
                  << "m_remoteSource=" << m_remoteSource
                  << "localSource=" << localSource;
        QFile localFile (localSource);
        if (localFile.exists ()) {
            update_localSource (QUrl::fromLocalFile (localSource).toString ());
            qDebug () << "file exists";
        }
        else {
            QNetworkReply * reply = SeriesCommon::getInstance ()->getNAM ()->get (QNetworkRequest (QUrl (m_remoteSource)));
            reply->setProperty ("localSource", localSource);
            connect (reply, &QNetworkReply::finished, this,  &SeriesImageCacher::onRequestFinished);
            qDebug () << "request img";
        }
    }
    else {
        update_localSource (QStringLiteral (""));
    }
}

void SeriesImageCacher::onRequestFinished () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ());
    if (reply && reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        if (!data.isEmpty ()) {
            QString localSource  = reply->property ("localSource").toString ();
            QFile localFile (localSource);
            if (localFile.open (QIODevice::WriteOnly | QIODevice::Truncate)) {
                localFile.write (data);
                localFile.flush ();
                localFile.close ();
            }
            update_localSource (QUrl::fromLocalFile (localSource).toString ());
        }
    }
}
