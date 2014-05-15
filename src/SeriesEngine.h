#ifndef SERIESENGINE_H
#define SERIESENGINE_H

#include <QObject>
#include <QThread>
#include <QVariant>

#include <QQmlHelpers>
#include <QQmlObjectListModel>

class SeriesWorker;

class SeriesEngine : public QObject {
    Q_OBJECT
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, searchModel)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, seriesModel)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, seasonsModel)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, episodesModel)

public:
    explicit SeriesEngine (QObject * parent = NULL);
    virtual ~SeriesEngine ();

public slots:
    void requestSearch          (QString name);
    void requestRemoveSerie     (QString serieId);
    void requestLoadSeasons     (QString serieId);
    void requestLoadEpisodes    (QString serieId, int seasonNumber);
    void requestToggleWatched   (QString serieId, int seasonNumber, int episodeNumber, bool watched);
    void requestFullSerieInfo   (QString serieId, QString title, QString overview, QString banner);

signals:
    void searchRequested        (QString name);
    void fullSerieInfoRequested (QString serieId, QString title, QString overview, QString banner);
    void removeSerieRequested   (QString serieId);
    void loadSeasonsRequested   (QString serieId);
    void loadEpisodesRequested  (QString serieId, int seasonNumber);
    void toggleWatchedRequested (QString serieId, int seasonNumber, int episodeNumber, bool watched);

private slots:
    void onSearchResultsUpdated (QVariantList list);
    void onSerieItemAdded       (QString      serieId);
    void onSerieItemRemoved     (QString      serieId);
    void onSerieItemUpdated     (QString      serieId,   QVariantMap values);
    void onSeasonItemAdded      (QString      seasonId);
    void onSeasonItemUpdated    (QString      seasonId,  QVariantMap values);
    void onEpisodeItemAdded     (QString      episodeId);
    void onEpisodeItemUpdated   (QString      episodeId, QVariantMap values);

private:
    SeriesWorker * m_worker;
    QThread      * m_thread;
};

#endif // SERIESENGINE_H
