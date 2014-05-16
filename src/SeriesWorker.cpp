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
        qDebug ("Offline storage database opened.");
        m_db.transaction ();
        m_db.exec ("CREATE TABLE IF NOT EXISTS series ( "
                   "     slug TEXT NOT NULL DEFAULT (''), "
                   "     title TEXT NOT NULL DEFAULT (''), "
                   "     banner TEXT NOT NULL DEFAULT (''), "
                   "     overview TEXT NOT NULL DEFAULT (''), "
                   "     fetched INTEGER NOT NULL DEFAULT (0), "
                   "     PRIMARY KEY (slug) "
                   " );");
        m_db.exec ("CREATE TABLE IF NOT EXISTS seasons ( "
                   "     slug TEXT NOT NULL DEFAULT (''), "
                   "     season INTEGER NOT NULL DEFAULT (0), "
                   "     poster TEXT NOT NULL DEFAULT (''), "
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
        loadSeriesFromDb ();
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

void SeriesWorker::getFullSerieInfo (QString serieId, QString title, QString overview, QString banner) {
    QSqlQuery queryAdd  (m_db);
    queryAdd.prepare    ("INSERT OR IGNORE INTO series (slug) VALUES (:slug)");
    queryAdd.bindValue  (":slug", serieId);
    queryAdd.exec       ();
    QSqlQuery queryEdit (m_db);
    queryEdit.prepare   ("UPDATE series SET title=:title, banner=:banner, overview=:overview WHERE slug=:slug");
    queryEdit.bindValue (":title",    title);
    queryEdit.bindValue (":banner",   banner);
    queryEdit.bindValue (":overview", overview);
    queryEdit.bindValue (":slug",     serieId);
    queryEdit.exec      ();
    QVariantMap values;
    values.insert ("title",    title);
    values.insert ("banner",   banner); // TODO : cache locally and update url later
    values.insert ("overview", overview);
    emit serieItemAdded   (serieId);
    emit serieItemUpdated (serieId, values);

    QNetworkRequest reqSeasons (QString ("%1/show/seasons.json/%2/%3").arg (traktApiUrl).arg (traktApiKey).arg (serieId));
    QNetworkReply * replySeason = m_nam->get (reqSeasons);
    replySeason->setProperty ("serieId", serieId);
    connect (replySeason, &QNetworkReply::finished, this, &SeriesWorker::onSeasonReply);
}

void SeriesWorker::loadSeriesFromDb () {
    QSqlQuery queryList (m_db);
    queryList.prepare ("SELECT slug, title, banner, overview, fetched FROM series GROUP BY slug ORDER BY title");
    if (queryList.exec ()) {
        QSqlRecord record      = queryList.record ();
        int fieldSerieSlug     = record.indexOf ("slug");
        int fieldSerieTitle    = record.indexOf ("title");
        int fieldSerieBanner   = record.indexOf ("banner");
        int fieldSerieOverview = record.indexOf ("overview");
        while (queryList.next ()) {
            QString serieId = queryList.value (fieldSerieSlug).toString ();
            QVariantMap values;
            values.insert ("serieId",  serieId);
            values.insert ("title",    queryList.value (fieldSerieTitle).toString ());
            values.insert ("banner",   queryList.value (fieldSerieBanner).toString ());
            values.insert ("overview", queryList.value (fieldSerieOverview).toString ());
            emit serieItemAdded   (serieId);
            emit serieItemUpdated (serieId, values);
        }
    }
}

void SeriesWorker::loadSeasonsFromDb (QString serieId) {
    QSqlQuery queryList (m_db);
    queryList.prepare ("SELECT slug, season, poster FROM seasons WHERE slug=:slug ORDER BY season");
    queryList.bindValue (":slug", serieId);
    if (queryList.exec ()) {
        QSqlRecord record          = queryList.record ();
        int fieldSerieSlug         = record.indexOf ("slug");
        int fieldSeasonNumber      = record.indexOf ("season");
        int fieldSeriePoster       = record.indexOf ("poster");
        while (queryList.next ()) {
            QString serieId      = queryList.value (fieldSerieSlug).toString ();
            int seasonNumber     = queryList.value (fieldSeasonNumber).toInt ();
            QString seasonId     = QString ("%1_S%2").arg (serieId).arg (seasonNumber);
            QVariantMap values;
            values.insert ("serieId",      serieId);
            values.insert ("seasonNumber", seasonNumber);
            values.insert ("seasonId",     seasonId);
            values.insert ("poster",       queryList.value (fieldSeriePoster).toString ());
            emit seasonItemAdded   (seasonId);
            emit seasonItemUpdated (seasonId, values);
        }
    }
}

void SeriesWorker::loadEpisodesFromDb (QString serieId, int seasonNumber) {
    QSqlQuery queryList (m_db);
    queryList.prepare ("SELECT slug, season, episode, title, overview, screen, watched FROM episodes WHERE slug=:slug AND season=:season ORDER BY episode");
    queryList.bindValue (":slug",   serieId);
    queryList.bindValue (":season", seasonNumber);
    if (queryList.exec ()) {
        QSqlRecord record        = queryList.record ();
        int fieldSerieSlug       = record.indexOf ("slug");
        int fieldSeasonNumber    = record.indexOf ("season");
        int fieldEpisodeNumber   = record.indexOf ("episode");
        int fieldEpisodeTitle    = record.indexOf ("title");
        int fieldEpisodeOverview = record.indexOf ("overview");
        int fieldEpisodeScreen   = record.indexOf ("screen");
        int fieldEpisodeWatched  = record.indexOf ("watched");
        int count = 0;
        while (queryList.next ()) {
            QString serieId      = queryList.value (fieldSerieSlug).toString ();
            int seasonNumber     = queryList.value (fieldSeasonNumber).toInt ();
            int episodeNumber    = queryList.value (fieldEpisodeNumber).toInt ();
            QString episodeId    = QString ("%1_S%2_E%3").arg (serieId).arg (seasonNumber).arg (episodeNumber);
            QVariantMap values;
            values.insert ("serieId",       serieId);
            values.insert ("seasonNumber",  seasonNumber);
            values.insert ("episodeNumber", episodeNumber);
            values.insert ("episodeId",     episodeId);
            values.insert ("title",         queryList.value (fieldEpisodeTitle).toString ());
            values.insert ("overview",      queryList.value (fieldEpisodeOverview).toString ());
            values.insert ("screen",        queryList.value (fieldEpisodeScreen).toString ());
            values.insert ("watched",       queryList.value (fieldEpisodeWatched).toBool ());
            emit episodeItemAdded   (episodeId);
            emit episodeItemUpdated (episodeId, values);
            count++;
        }
        QVariantMap season;
        season.insert ("episodeCount", count);
        emit seasonItemUpdated (QString ("%1_S%2").arg (serieId).arg (seasonNumber), season);
    }
}

void SeriesWorker::toggleEpisodeWatched (QString serieId, int seasonNumber, int episodeNumber, bool watched) {
    QString episodeId = QString ("%1_S%2_E%3").arg (serieId).arg (seasonNumber).arg (episodeNumber);
    m_db.transaction ();
    QSqlQuery queryToggle (m_db);
    queryToggle.prepare   ("UPDATE episodes SET watched=:watched WHERE slug=:slug AND season=:season AND episode=:episode");
    queryToggle.bindValue (":watched", (watched ? 1 : 0));
    queryToggle.bindValue (":slug",    serieId);
    queryToggle.bindValue (":season",  seasonNumber);
    queryToggle.bindValue (":episode", episodeNumber);
    queryToggle.exec      ();
    m_db.commit ();
    QVariantMap episode;
    episode.insert ("watched", watched);
    emit episodeItemUpdated (episodeId, episode);
}

void SeriesWorker::removeSerieInfo (QString serieId) {
    m_db.transaction ();
    QSqlQuery queryRemoveSerie    (m_db);
    queryRemoveSerie.prepare      ("DELETE FROM series WHERE slug=:slug");
    queryRemoveSerie.bindValue    (":slug", serieId);
    queryRemoveSerie.exec         ();
    QSqlQuery queryRemoveSeasons  (m_db);
    queryRemoveSeasons.prepare    ("DELETE FROM seasons WHERE slug=:slug");
    queryRemoveSeasons.bindValue  (":slug", serieId);
    queryRemoveSeasons.exec       ();
    QSqlQuery queryRemoveEpisodes (m_db);
    queryRemoveEpisodes.prepare   ("DELETE FROM episodes WHERE slug=:slug");
    queryRemoveEpisodes.bindValue (":slug", serieId);
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
                qDebug () << "searchForSerie : proposal=" << serie;
                QVariantMap item;
                item.insert ("title",    serie.value ("title").toString ());
                item.insert ("banner",   serie.value ("images").toMap ().value ("banner").toString ());
                item.insert ("overview", serie.value ("overview").toString ());
                item.insert ("serieId",  serie.value ("url").toString ().split ("/").last ());
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
            queryAdd.prepare ("INSERT OR IGNORE INTO seasons (slug, season) VALUES (:slug, :season)");

            QSqlQuery queryEdit (m_db);
            queryEdit.prepare ("UPDATE seasons SET poster=:poster WHERE slug=:slug AND season=:season");

            QVariantList list = json.array ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap season = value.toMap ();

                int seasonNumber  = season.value ("season").toInt ();
                int episodeCount  = season.value ("episodes").toInt ();
                QString posterUrl = season.value ("poster").toString ();

                qDebug () << "onSeasonReply : season=" << season;

                queryAdd.bindValue (":slug",   serieId);
                queryAdd.bindValue (":season", seasonNumber);
                queryAdd.exec ();

                queryEdit.bindValue (":poster",        posterUrl);
                queryEdit.bindValue (":slug",          serieId);
                queryEdit.bindValue (":season",        seasonNumber);
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
        QString serieId = reply->property ("serieId").toString ();
        int seasonNumber = reply->property ("seasonNumber").toInt ();
        QByteArray data = reply->readAll ();
        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isArray ()) {
            m_db.transaction ();

            QSqlQuery queryAdd (m_db);
            queryAdd.prepare ("INSERT OR IGNORE INTO episodes (slug, season, episode) VALUES (:slug, :season, :episode)");

            QSqlQuery queryEdit (m_db);
            queryEdit.prepare ("UPDATE episodes SET title=:title, overview=:overview, screen=:screen WHERE slug=:slug AND season=:season AND episode=:episode");

            QVariantList list = json.array ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap values = value.toMap ();

                int     episodeNumber = values.value ("episode").toInt ();
                QString title         = values.value ("title").toString ();
                QString overview      = values.value ("overview").toString ();
                QString screen        = values.value ("screen").toString ();

                qDebug () << "onEpisodesReply : episode=" << values;

                queryAdd.bindValue (":slug",    serieId);
                queryAdd.bindValue (":season",  seasonNumber);
                queryAdd.bindValue (":episode", episodeNumber);
                queryAdd.exec ();

                queryEdit.bindValue (":title",    title);
                queryEdit.bindValue (":overview", overview);
                queryEdit.bindValue (":screen",   screen);
                queryEdit.bindValue (":slug",     serieId);
                queryEdit.bindValue (":season",   seasonNumber);
                queryEdit.bindValue (":episode",  episodeNumber);
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
