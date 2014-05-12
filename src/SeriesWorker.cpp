#include "SeriesWorker.h"

#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>

SeriesWorker::SeriesWorker (QObject * parent) : QObject (parent) {
    m_db  = QSqlDatabase::addDatabase (QStringLiteral ("QSQLITE"));
    m_nam = new QNetworkAccessManager (this);
}

void SeriesWorker::initialize () {
    QString path (QStandardPaths::writableLocation (QStandardPaths::DataLocation));
    QDir dir (QDir::homePath ());
    dir.mkpath (path);
    m_db.setHostName (QStringLiteral ("localhost"));
    m_db.setDatabaseName (QString ("%1/series_storage.db").arg (path));
    if (m_db.open ()) {
        qDebug ("Offline storage database opened.");
        m_db.transaction ();
        m_db.exec ("CREATE TABLE IF NOT EXISTS series ( "
                   "     slug TEXT NOT NULL DEFAULT (''), "
                   "     title TEXT NOT NULL DEFAULT (''), "
                   "     banner TEXT NOT NULL DEFAULT (''), "
                   "     tvdb_id INTEGER NOT NULL DEFAULT (-1), "
                   "     overview TEXT NOT NULL DEFAULT (''), "
                   "     fetched INTEGER NOT NULL DEFAULT (0), "
                   "     PRIMARY KEY (slug) "
                   " );");
        m_db.exec ("CREATE TABLE IF NOT EXISTS seasons ( "
                   "     slug TEXT NOT NULL DEFAULT (''), "
                   "     season INTEGER NOT NULL DEFAULT (0), "
                   "     poster TEXT NOT NULL DEFAULT (''), "
                   "     episode_count INTEGER NOT NULL DEFAULT (0), "
                   "     fetched INTEGER NOT NULL DEFAULT (0), "
                   "     PRIMARY KEY (slug, season) "
                   " );");
        m_db.exec ("CREATE TABLE IF NOT EXISTS episodes ( "
                   "     slug TEXT NOT NULL DEFAULT (''), "
                   "     season INTEGER NOT NULL DEFAULT (0), "
                   "     episode INTEGER NOT NULL DEFAULT (0), "
                   "     title TEXT NOT NULL DEFAULT (''), "
                   "     overview TEXT NOT NULL DEFAULT (''), "
                   "     screen TEXT NOT NULL DEFAULT (''), "
                   "     watched INTEGER NOT NULL DEFAULT (0), "
                   "     PRIMARY KEY (slug, season, episode) "
                   " );");
        m_db.commit ();
    }
    else {
        qWarning () << "Offline storage database couldn't be loaded nor created !"
                    << m_db.lastError ().text ();
    }
}

void SeriesWorker::searchForSerie (QString name) {
    qDebug () << "Searching for serie" << name << "...";
    QNetworkRequest request (QString ("%1/search/shows.json/%2/%3").arg (traktApiUrl).arg (traktApiKey).arg (name.replace (" ", "+")));
    QNetworkReply * reply = m_nam->get (request);
    connect (reply, &QNetworkReply::finished, this, &SeriesWorker::onSearchReply);
}

void SeriesWorker::getFullSerieInfo (QString slug, QString title, QString overview, QString banner, int tvdb_id) {
    m_db.transaction ();
    QSqlQuery queryAdd ("INSERT OR IGNORE INTO series (slug) VALUES (:slug)", m_db);
    queryAdd.bindValue (":slug", slug);
    queryAdd.exec ();
    emit serieItemAdded (slug);
    QSqlQuery queryEdit ("UPDATE series SET title=:title, banner=:banner, overview=:overview, tvdb_id=:tvdb_id  WHERE slug=:slug", m_db);
    queryEdit.bindValue (":title",    title);
    queryEdit.bindValue (":banner",   banner);
    queryEdit.bindValue (":overview", overview);
    queryEdit.bindValue (":tvdb_id",  tvdb_id);
    queryEdit.bindValue (":slug",     slug);
    queryEdit.exec ();
    QVariantMap values;
    values.insert ("title", title);
    values.insert ("banner", banner); // TODO : cache locally and update url later
    values.insert ("overview", overview);
    values.insert ("tvdb_id", tvdb_id);
    emit serieItemUpdated (slug, values);
    m_db.commit ();


}

void SeriesWorker::onSearchReply () {
    QVariantList ret;
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    if (reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isArray ()) {
            QVariantList list = json.array ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap serie = value.toMap ();
                qDebug () << "searchForSerie : proposal=" << serie;
                QVariantMap item;
                item.insert ("title",    serie.value ("title").toString ());
                item.insert ("banner",   serie.value ("images").toMap ().value ("banner").toString ());
                item.insert ("tvdb_id",  serie.value ("tvdb_id").toInt ());
                item.insert ("overview", serie.value ("overview").toString ());
                item.insert ("slug",     serie.value ("url").toString ().split ("/").last ());
                ret.append (item);
            }
        }
        else {
            qWarning () << "Search show : result is not an array !";
        }
    }
    else {
        qWarning () << "Network error on search request :" << reply->errorString ();
    }
    emit searchResultsUpdated (ret);
}
