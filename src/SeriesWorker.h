#ifndef SERIESWORKER_H
#define SERIESWORKER_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QNetworkAccessManager>

#define traktApiUrl QStringLiteral ("http://api.trakt.tv")
#define traktApiKey QStringLiteral ("46f39bcb258d9684f4edf927e4c06c52")

class SeriesWorker : public QObject {
    Q_OBJECT
public:
    explicit SeriesWorker (QObject * parent = NULL);

signals:
    void searchResultsUpdated (QVariantList list);
    void serieItemAdded       (QString      serieId);
    void serieItemRemoved     (QString      serieId);
    void serieItemUpdated     (QString      serieId,   QVariantMap values);
    void seasonItemAdded      (QString      seasonId);
    void seasonItemUpdated    (QString      seasonId,  QVariantMap values);
    void episodeItemAdded     (QString      episodeId);
    void episodeItemUpdated   (QString      episodeId, QVariantMap values);

public slots:
    void initialize           ();
    void loadSeriesFromDb     ();
    void loadSeasonsFromDb    (QString      serieId);
    void loadEpisodesFromDb   (QString      serieId, int seasonNumber);
    void toggleEpisodeWatched (QString      serieId, int seasonNumber, int episodeNumber, bool watched);
    void removeSerieInfo      (QString      serieId);
    void searchForSerie       (QString      name);
    void getFullSerieInfo     (QString      serieId, QString title, QString overview, QString banner);

private slots:
    void onSearchReply   ();
    void onSeasonReply   ();
    void onEpisodesReply ();

private:
    QSqlDatabase            m_db;
    QNetworkAccessManager * m_nam;
};

#endif // SERIESWORKER_H
