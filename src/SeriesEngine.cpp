
#include "SeriesEngine.h"
#include "SeriesWorker.h"
#include "SeriesItem.h"
#include "SeriesSeason.h"
#include "SeriesEpisode.h"

#include <QDebug>

SeriesEngine::SeriesEngine (QObject * parent) : QObject (parent) {
    m_searchModel   = QQmlObjectListModel::create<SeriesItem>    (this);
    m_seriesModel   = QQmlObjectListModel::create<SeriesItem>    (this);
    m_seasonsModel  = QQmlObjectListModel::create<SeriesSeason>  (this);
    m_episodesModel = QQmlObjectListModel::create<SeriesEpisode> (this);

    m_seriesModel->setRoleNameForUid   (QByteArrayLiteral ("serieId"));
    m_seasonsModel->setRoleNameForUid  (QByteArrayLiteral ("seasonId"));
    m_episodesModel->setRoleNameForUid (QByteArrayLiteral ("episodeId"));

    m_thread = new QThread (this);

    m_worker = new SeriesWorker;
    m_worker->moveToThread (m_thread);

    connect (m_thread,  &QThread::started,                     m_worker, &SeriesWorker::initialize);

    // requests
    connect (this,      &SeriesEngine::searchRequested,        m_worker, &SeriesWorker::searchForSerie);
    connect (this,      &SeriesEngine::fullSerieInfoRequested, m_worker, &SeriesWorker::getFullSerieInfo);
    connect (this,      &SeriesEngine::loadSeasonsRequested,   m_worker, &SeriesWorker::loadSeasonsFromDb);
    connect (this,      &SeriesEngine::loadEpisodesRequested,  m_worker, &SeriesWorker::loadEpisodesFromDb);
    connect (this,      &SeriesEngine::removeSerieRequested,   m_worker, &SeriesWorker::removeSerieInfo);
    connect (this,      &SeriesEngine::toggleWatchedRequested, m_worker, &SeriesWorker::toggleEpisodeWatched);

    // replies
    connect (m_worker,  &SeriesWorker::searchResultsUpdated,   this,     &SeriesEngine::onSearchResultsUpdated);
    connect (m_worker,  &SeriesWorker::serieItemAdded,         this,     &SeriesEngine::onSerieItemAdded);
    connect (m_worker,  &SeriesWorker::serieItemRemoved,       this,     &SeriesEngine::onSerieItemRemoved);
    connect (m_worker,  &SeriesWorker::serieItemUpdated,       this,     &SeriesEngine::onSerieItemUpdated);
    connect (m_worker,  &SeriesWorker::seasonItemAdded,        this,     &SeriesEngine::onSeasonItemAdded);
    connect (m_worker,  &SeriesWorker::seasonItemUpdated,      this,     &SeriesEngine::onSeasonItemUpdated);
    connect (m_worker,  &SeriesWorker::episodeItemAdded,       this,     &SeriesEngine::onEpisodeItemAdded);
    connect (m_worker,  &SeriesWorker::episodeItemUpdated,     this,     &SeriesEngine::onEpisodeItemUpdated);

    m_thread->start (QThread::HighestPriority);
}

SeriesEngine::~SeriesEngine () {
    m_thread->quit ();
    m_thread->wait ();
}

void SeriesEngine::requestSearch (QString name) {
    emit searchRequested (name);
}

void SeriesEngine::requestRemoveSerie (QString serieId) {
    emit removeSerieRequested (serieId);
}

void SeriesEngine::requestLoadSeasons (QString serieId) {
    m_seasonsModel->clear ();
    m_episodesModel->clear ();
    emit loadSeasonsRequested (serieId);
}

void SeriesEngine::requestLoadEpisodes (QString serieId, int seasonNumber) {
    m_episodesModel->clear ();
    emit loadEpisodesRequested (serieId, seasonNumber);
}

void SeriesEngine::requestFullSerieInfo (QString serieId, QString title, QString overview, QString banner) {
    emit fullSerieInfoRequested (serieId, title, overview, banner);
}

void SeriesEngine::requestToggleWatched (QString serieId, int seasonNumber, int episodeNumber, bool watched) {
    qDebug () << "toggle episode watched=" << serieId << seasonNumber << episodeNumber << watched;
    emit toggleWatchedRequested (serieId, seasonNumber, episodeNumber, watched);
}

void SeriesEngine::onSearchResultsUpdated (QVariantList list) {
    qDebug () << "search results=" << list.length ();
    m_searchModel->clear ();
    foreach (QVariant variant, list) {
        QVariantMap values = variant.toMap ();
        qDebug () << "values=" << values;
        SeriesItem * item = SeriesItem::fromQtVariant (values);
        qDebug () << "item="<< item->get_serieId () << item->get_title () << item->get_banner ();
        m_searchModel->append (item);
    }
    qDebug () << "search count=" << m_searchModel->count ();
}

void SeriesEngine::onSerieItemAdded (QString serieId) {
    qDebug () << "serie item added=" << serieId;
    if (!m_seriesModel->getByUid (serieId)) {
        SeriesItem * item = new SeriesItem;
        item->update_serieId (serieId);
        m_seriesModel->append (item);
    }
}

void SeriesEngine::onSerieItemRemoved (QString serieId) {
    qDebug () << "serie item removed=" << serieId;
    QObject * item = m_seriesModel->getByUid (serieId);
    if (item) {
        m_seriesModel->remove (item);
    }
}

void SeriesEngine::onSerieItemUpdated (QString serieId, QVariantMap values) {
    qDebug () << "serie item updated=" << serieId << values;
    onSerieItemAdded (serieId);
    SeriesItem * item = qobject_cast<SeriesItem *> (m_seriesModel->getByUid (serieId));
    if (item) {
        item->updateWithQtVariant (values);
    }
}

void SeriesEngine::onSeasonItemAdded (QString seasonId) {
    qDebug () << "season item added=" << seasonId;
    if (!m_seasonsModel->getByUid (seasonId)) {
        SeriesSeason * item = new SeriesSeason;
        item->update_seasonId (seasonId);
        m_seasonsModel->append (item);
    }
}

void SeriesEngine::onSeasonItemUpdated (QString seasonId, QVariantMap values) {
    qDebug () << "season item updated=" << seasonId << values;
    onSeasonItemAdded (seasonId);
    SeriesSeason * item = qobject_cast<SeriesSeason *> (m_seasonsModel->getByUid (seasonId));
    if (item) {
        item->updateWithQtVariant (values);
    }
}

void SeriesEngine::onEpisodeItemAdded (QString episodeId) {
    qDebug () << "episode item added=" << episodeId;
    if (!m_episodesModel->getByUid (episodeId)) {
        SeriesEpisode * item = new SeriesEpisode;
        item->update_episodeId (episodeId);
        m_episodesModel->append (item);
    }
}

void SeriesEngine::onEpisodeItemUpdated (QString episodeId, QVariantMap values) {
    qDebug () << "episode item updated=" << episodeId << values;
    onEpisodeItemAdded (episodeId);
    SeriesEpisode * item = qobject_cast<SeriesEpisode *> (m_episodesModel->getByUid (episodeId));
    if (item) {
        item->updateWithQtVariant (values);
    }
}
