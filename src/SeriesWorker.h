#ifndef SERIESWORKER_H
#define SERIESWORKER_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QNetworkAccessManager>
#include <QSslSocket>
#include <QTcpServer>

static const QByteArray & traktApiClientId     = QByteArrayLiteral ("86ccde896d88b8bee52c0ed37f05c1b4f47705adbc99a14543b32818001cdf55");
static const QByteArray & traktApiClientSecret = QByteArrayLiteral ("8004d8f17b14829258bb79025c772447efe9cba611b7a938643f024d4a6038b4");

class SeriesWorker;

typedef void (SeriesWorker::*WorkerCallback) (void);

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

protected:
    void doHttpGetRequest (QString url, WorkerCallback callback, QVariantMap payload = QVariantMap ());

private slots:
    void onSearchReply   ();
    void onSeasonReply   ();

private:
    QSqlDatabase            m_db;
    QNetworkAccessManager * m_nam;
};

#endif // SERIESWORKER_H
