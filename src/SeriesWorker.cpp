#include "SeriesWorker.h"

#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
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
    dir.mkpath (QString ("%1/images").arg (path));
    m_db.setHostName (QStringLiteral ("localhost"));
    m_db.setDatabaseName (QString ("%1/series_storage.db").arg (path));
    if (m_db.open ()) {
        //qDebug ("Offline storage database opened.");
        m_db.transaction ();
        m_db.exec (QStringLiteral ("CREATE TABLE IF NOT EXISTS series ( "
                                   "     slug TEXT NOT NULL DEFAULT (''), "
                                   "     title TEXT NOT NULL DEFAULT (''), "
                                   "     banner TEXT NOT NULL DEFAULT (''), "
                                   "     overview TEXT NOT NULL DEFAULT (''), "
                                   "     fetched INTEGER NOT NULL DEFAULT (0), "
                                   "     PRIMARY KEY (slug) "
                                   " );"));
        m_db.exec (QStringLiteral ("CREATE TABLE IF NOT EXISTS seasons ( "
                                   "     slug TEXT NOT NULL DEFAULT (''), "
                                   "     season INTEGER NOT NULL DEFAULT (0), "
                                   "     poster TEXT NOT NULL DEFAULT (''), "
                                   "     PRIMARY KEY (slug, season) "
                                   " );"));
        m_db.exec (QStringLiteral ("CREATE TABLE IF NOT EXISTS episodes ( "
                                   "     slug TEXT NOT NULL DEFAULT (''), "
                                   "     season INTEGER NOT NULL DEFAULT (0), "
                                   "     episode INTEGER NOT NULL DEFAULT (0), "
                                   "     title TEXT NOT NULL DEFAULT (''), "
                                   "     overview TEXT NOT NULL DEFAULT (''), "
                                   "     screen TEXT NOT NULL DEFAULT (''), "
                                   "     watched INTEGER NOT NULL DEFAULT (0), "
                                   "     PRIMARY KEY (slug, season, episode) "
                                   " );"));
        m_db.commit ();
        loadSeriesFromDb ();
    }
    else {
        qWarning () << "Offline storage database couldn't be loaded nor created !"
                    << m_db.lastError ().text ();
    }
}

void SeriesWorker::searchForSerie (QString name) {
    //qDebug () << "Searching for serie" << name << "...";
    QNetworkRequest request (QString ("%1/search/shows.json/%2/%3").arg (traktApiUrl).arg (traktApiKey).arg (name.replace (" ", "+")));
    QNetworkReply * reply = m_nam->get (request);
    connect (reply, &QNetworkReply::finished, this, &SeriesWorker::onSearchReply);
}

void SeriesWorker::getFullSerieInfo (QString serieId, QString title, QString overview, QString banner) {
    m_db.transaction ();
    QSqlQuery queryAdd  (m_db);
    queryAdd.prepare    (QStringLiteral ("INSERT OR IGNORE INTO series (slug) VALUES (:slug)"));
    queryAdd.bindValue  (QStringLiteral (":slug"), serieId);
    queryAdd.exec       ();
    QSqlQuery queryEdit (m_db);
    queryEdit.prepare   (QStringLiteral ("UPDATE series SET title=:title, banner=:banner, overview=:overview WHERE slug=:slug"));
    queryEdit.bindValue (QStringLiteral (":title"),    title);
    queryEdit.bindValue (QStringLiteral (":banner"),   banner);
    queryEdit.bindValue (QStringLiteral (":overview"), overview);
    queryEdit.bindValue (QStringLiteral (":slug"),     serieId);
    queryEdit.exec      ();
    m_db.commit ();

    QVariantMap values;
    values.insert (QStringLiteral ("title"),    title);
    values.insert (QStringLiteral ("banner"),   banner); // TODO : cache locally and update url later
    values.insert (QStringLiteral ("overview"), overview);
    emit serieItemAdded   (serieId);
    emit serieItemUpdated (serieId, values);

    QNetworkRequest reqSeasons (QString ("%1/show/seasons.json/%2/%3").arg (traktApiUrl).arg (traktApiKey).arg (serieId));
    QNetworkReply * replySeason = m_nam->get (reqSeasons);
    replySeason->setProperty ("serieId", serieId);
    connect (replySeason, &QNetworkReply::finished, this, &SeriesWorker::onSeasonReply);
}

void SeriesWorker::loadSeriesFromDb () {
    QSqlQuery queryList (m_db);
    queryList.prepare (QStringLiteral ("SELECT slug, title, banner, overview, fetched FROM series GROUP BY slug ORDER BY title"));
    if (queryList.exec ()) {
        QSqlRecord record      = queryList.record ();
        int fieldSerieSlug     = record.indexOf (QStringLiteral ("slug"));
        int fieldSerieTitle    = record.indexOf (QStringLiteral ("title"));
        int fieldSerieBanner   = record.indexOf (QStringLiteral ("banner"));
        int fieldSerieOverview = record.indexOf (QStringLiteral ("overview"));
        while (queryList.next ()) {
            QString serieId = queryList.value (fieldSerieSlug).toString ();
            QVariantMap values;
            values.insert (QStringLiteral ("serieId"),  serieId);
            values.insert (QStringLiteral ("title"),    queryList.value (fieldSerieTitle).toString ());
            values.insert (QStringLiteral ("banner"),   queryList.value (fieldSerieBanner).toString ());
            values.insert (QStringLiteral ("overview"), queryList.value (fieldSerieOverview).toString ());
            emit serieItemAdded   (serieId);
            emit serieItemUpdated (serieId, values);
        }
    }
}

void SeriesWorker::loadSeasonsFromDb (QString serieId) {
    QSqlQuery queryList (m_db);
    queryList.prepare (QStringLiteral ("SELECT slug, season, poster, "
                       "    (SELECT count(*) FROM episodes WHERE seasons.season=episodes.season AND seasons.slug=episodes.slug) as episodeCount, "
                       "    (SELECT count(*) FROM episodes WHERE seasons.season=episodes.season AND seasons.slug=episodes.slug AND episodes.watched='1') as watchedCount "
                       "FROM seasons WHERE slug=:slug ORDER BY season"));
    queryList.bindValue (QStringLiteral (":slug"), serieId);
    if (queryList.exec ()) {
        QSqlRecord record          = queryList.record ();
        int fieldSerieSlug         = record.indexOf (QStringLiteral ("slug"));
        int fieldSeasonNumber      = record.indexOf (QStringLiteral ("season"));
        int fieldSeasonPoster      = record.indexOf (QStringLiteral ("poster"));
        int fieldEpisodeCount      = record.indexOf (QStringLiteral ("episodeCount"));
        int fieldWatchedCount      = record.indexOf (QStringLiteral ("watchedCount"));
        while (queryList.next ()) {
            QString serieId      = queryList.value (fieldSerieSlug).toString ();
            int seasonNumber     = queryList.value (fieldSeasonNumber).toInt ();
            QString seasonId     = QString ("%1_S%2").arg (serieId).arg (seasonNumber);
            QVariantMap values;
            values.insert (QStringLiteral ("serieId"),       serieId);
            values.insert (QStringLiteral ("seasonNumber"),  seasonNumber);
            values.insert (QStringLiteral ("seasonId"),      seasonId);
            values.insert (QStringLiteral ("poster"),        queryList.value (fieldSeasonPoster).toString ());
            values.insert (QStringLiteral ("episodeCount"),  queryList.value (fieldEpisodeCount).toInt ());
            values.insert (QStringLiteral ("watchedCount"),  queryList.value (fieldWatchedCount).toInt ());
            emit seasonItemAdded   (seasonId);
            emit seasonItemUpdated (seasonId, values);
        }
    }
}

void SeriesWorker::loadEpisodesFromDb (QString serieId, int seasonNumber) {
    QSqlQuery queryList (m_db);
    queryList.prepare (QStringLiteral ("SELECT slug, season, episode, title, overview, screen, watched FROM episodes "
                                       "WHERE slug=:slug AND season=:season ORDER BY episode"));
    queryList.bindValue (QStringLiteral (":slug"),   serieId);
    queryList.bindValue (QStringLiteral (":season"), seasonNumber);
    if (queryList.exec ()) {
        QSqlRecord record        = queryList.record ();
        int fieldSerieSlug       = record.indexOf (QStringLiteral ("slug"));
        int fieldSeasonNumber    = record.indexOf (QStringLiteral ("season"));
        int fieldEpisodeNumber   = record.indexOf (QStringLiteral ("episode"));
        int fieldEpisodeTitle    = record.indexOf (QStringLiteral ("title"));
        int fieldEpisodeOverview = record.indexOf (QStringLiteral ("overview"));
        int fieldEpisodeScreen   = record.indexOf (QStringLiteral ("screen"));
        int fieldEpisodeWatched  = record.indexOf (QStringLiteral ("watched"));
        while (queryList.next ()) {
            QString serieId      = queryList.value (fieldSerieSlug).toString ();
            int seasonNumber     = queryList.value (fieldSeasonNumber).toInt ();
            int episodeNumber    = queryList.value (fieldEpisodeNumber).toInt ();
            QString episodeId    = QString ("%1_S%2_E%3").arg (serieId).arg (seasonNumber).arg (episodeNumber);
            QVariantMap values;
            values.insert (QStringLiteral ("serieId"),       serieId);
            values.insert (QStringLiteral ("seasonNumber"),  seasonNumber);
            values.insert (QStringLiteral ("episodeNumber"), episodeNumber);
            values.insert (QStringLiteral ("episodeId"),     episodeId);
            values.insert (QStringLiteral ("title"),         queryList.value (fieldEpisodeTitle).toString ());
            values.insert (QStringLiteral ("overview"),      queryList.value (fieldEpisodeOverview).toString ());
            values.insert (QStringLiteral ("screen"),        queryList.value (fieldEpisodeScreen).toString ());
            values.insert (QStringLiteral ("watched"),       queryList.value (fieldEpisodeWatched).toBool ());
            emit episodeItemAdded   (episodeId);
            emit episodeItemUpdated (episodeId, values);
        }
    }
}

void SeriesWorker::toggleEpisodeWatched (QString serieId, int seasonNumber, int episodeNumber, bool watched) {
    QString episodeId = QString ("%1_S%2_E%3").arg (serieId).arg (seasonNumber).arg (episodeNumber);
    m_db.transaction ();
    QSqlQuery queryToggle (m_db);
    queryToggle.prepare   (QStringLiteral ("UPDATE episodes SET watched=:watched WHERE slug=:slug AND season=:season AND episode=:episode"));
    queryToggle.bindValue (QStringLiteral (":watched"), (watched ? 1 : 0));
    queryToggle.bindValue (QStringLiteral (":slug"),    serieId);
    queryToggle.bindValue (QStringLiteral (":season"),  seasonNumber);
    queryToggle.bindValue (QStringLiteral (":episode"), episodeNumber);
    queryToggle.exec      ();
    m_db.commit ();
    QVariantMap episode;
    episode.insert (QStringLiteral ("watched"), watched);
    emit episodeItemUpdated (episodeId, episode);
    loadSeasonsFromDb (serieId);
}

void SeriesWorker::removeSerieInfo (QString serieId) {
    m_db.transaction ();
    QSqlQuery queryRemoveSerie    (m_db);
    queryRemoveSerie.prepare      (QStringLiteral ("DELETE FROM series WHERE slug=:slug"));
    queryRemoveSerie.bindValue    (QStringLiteral (":slug"), serieId);
    queryRemoveSerie.exec         ();
    QSqlQuery queryRemoveSeasons  (m_db);
    queryRemoveSeasons.prepare    (QStringLiteral ("DELETE FROM seasons WHERE slug=:slug"));
    queryRemoveSeasons.bindValue  (QStringLiteral (":slug"), serieId);
    queryRemoveSeasons.exec       ();
    QSqlQuery queryRemoveEpisodes (m_db);
    queryRemoveEpisodes.prepare   (QStringLiteral ("DELETE FROM episodes WHERE slug=:slug"));
    queryRemoveEpisodes.bindValue (QStringLiteral (":slug"), serieId);
    queryRemoveEpisodes.exec      ();
    m_db.commit ();
    emit serieItemRemoved (serieId);
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
                QVariantMap item;
                item.insert (QStringLiteral ("title"),    serie.value (QStringLiteral ("title")).toString ());
                item.insert (QStringLiteral ("banner"),   serie.value (QStringLiteral ("images")).toMap ().value (QStringLiteral ("banner")).toString ());
                item.insert (QStringLiteral ("overview"), serie.value (QStringLiteral ("overview")).toString ());
                item.insert (QStringLiteral ("serieId"),  serie.value (QStringLiteral ("url")).toString ().split (QChar ('/')).last ());
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

void SeriesWorker::onSeasonReply () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    if (reply->error () == QNetworkReply::NoError) {
        QString serieId = reply->property ("serieId").toString ();
        QByteArray data = reply->readAll ();
        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isArray ()) {
            m_db.transaction ();

            QSqlQuery queryAdd (m_db);
            queryAdd.prepare (QStringLiteral ("INSERT OR IGNORE INTO seasons (slug, season) VALUES (:slug, :season)"));

            QSqlQuery queryEdit (m_db);
            queryEdit.prepare (QStringLiteral ("UPDATE seasons SET poster=:poster WHERE slug=:slug AND season=:season"));

            QVariantList list = json.array ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap season = value.toMap ();

                int seasonNumber  = season.value (QStringLiteral ("season")).toInt ();
                QString posterUrl = season.value (QStringLiteral ("poster")).toString ();

                //qDebug () << "onSeasonReply : season=" << season;

                queryAdd.bindValue (QStringLiteral (":slug"),   serieId);
                queryAdd.bindValue (QStringLiteral (":season"), seasonNumber);
                queryAdd.exec ();

                queryEdit.bindValue (QStringLiteral (":poster"),        posterUrl);
                queryEdit.bindValue (QStringLiteral (":slug"),          serieId);
                queryEdit.bindValue (QStringLiteral (":season"),        seasonNumber);
                queryEdit.exec ();


                QNetworkRequest reqEpisodes (QString ("%1/show/season.json/%2/%3/%4").arg (traktApiUrl).arg (traktApiKey).arg (serieId).arg (seasonNumber));
                QNetworkReply * replyEpisodes = m_nam->get (reqEpisodes);
                replyEpisodes->setProperty ("serieId",      serieId);
                replyEpisodes->setProperty ("seasonNumber", seasonNumber);
                connect (replyEpisodes, &QNetworkReply::finished, this, &SeriesWorker::onEpisodesReply);
            }
            m_db.commit ();
        }
        else {
            qWarning () << "Request seasons : result is not an array !";
        }
    }
    else {
        qWarning () << "Network error on seasons request :" << reply->errorString ();
    }
}

void SeriesWorker::onEpisodesReply () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    if (reply->error () == QNetworkReply::NoError) {
        QString serieId  = reply->property ("serieId").toString ();
        int seasonNumber = reply->property ("seasonNumber").toInt ();
        QByteArray data  = reply->readAll ();
        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isArray ()) {
            m_db.transaction ();

            QSqlQuery queryAdd (m_db);
            queryAdd.prepare (QStringLiteral ("INSERT OR IGNORE INTO episodes (slug, season, episode) VALUES (:slug, :season, :episode)"));

            QSqlQuery queryEdit (m_db);
            queryEdit.prepare (QStringLiteral ("UPDATE episodes SET title=:title, overview=:overview, screen=:screen WHERE slug=:slug AND season=:season AND episode=:episode"));

            QVariantList list = json.array ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap values = value.toMap ();

                int     episodeNumber = values.value (QStringLiteral ("episode")).toInt ();
                QString title         = values.value (QStringLiteral ("title")).toString ();
                QString overview      = values.value (QStringLiteral ("overview")).toString ();
                QString screen        = values.value (QStringLiteral ("screen")).toString ();

                //qDebug () << "onEpisodesReply : episode=" << values;

                queryAdd.bindValue (QStringLiteral (":slug"),    serieId);
                queryAdd.bindValue (QStringLiteral (":season"),  seasonNumber);
                queryAdd.bindValue (QStringLiteral (":episode"), episodeNumber);
                queryAdd.exec ();

                queryEdit.bindValue (QStringLiteral (":title"),    title);
                queryEdit.bindValue (QStringLiteral (":overview"), overview);
                queryEdit.bindValue (QStringLiteral (":screen"),   screen);
                queryEdit.bindValue (QStringLiteral (":slug"),     serieId);
                queryEdit.bindValue (QStringLiteral (":season"),   seasonNumber);
                queryEdit.bindValue (QStringLiteral (":episode"),  episodeNumber);
                queryEdit.exec ();
            }
            m_db.commit ();
        }
        else {
            qWarning () << "Request episodes : result is not an array !";
        }
    }
    else {
        qWarning () << "Network error on episodes request :" << reply->errorString ();
    }
}
