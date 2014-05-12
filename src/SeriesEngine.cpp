
#include "SeriesEngine.h"
#include "SeriesWorker.h"
#include "SeriesItem.h"

#include <QDebug>

SeriesEngine::SeriesEngine (QObject * parent) : QObject (parent) {
    m_searchModel = QQmlObjectListModel::create<SeriesItem> (this);
    m_searchModel->setRoleNameForUid (QByteArrayLiteral ("slug"));
    m_seriesModel = QQmlObjectListModel::create<SeriesItem> (this);
    m_seriesModel->setRoleNameForUid (QByteArrayLiteral ("slug"));
    m_thread = new QThread (this);
    m_worker = new SeriesWorker;
    m_worker->moveToThread (m_thread);

    connect (m_thread,  &QThread::started,                     m_worker, &SeriesWorker::initialize);

    // requests
    connect (this,      &SeriesEngine::searchRequested,        m_worker, &SeriesWorker::searchForSerie);
    connect (this,      &SeriesEngine::fullSerieInfoRequested, m_worker, &SeriesWorker::getFullSerieInfo);

    // replies
    connect (m_worker,  &SeriesWorker::searchResultsUpdated,   this,     &SeriesEngine::onSearchResultsUpdated);
    connect (m_worker,  &SeriesWorker::serieItemAdded,         this,     &SeriesEngine::onSerieItemAdded);
    connect (m_worker,  &SeriesWorker::serieItemUpdated,       this,     &SeriesEngine::onSerieItemUpdated);

    m_thread->start (QThread::HighestPriority);
}

SeriesEngine::~SeriesEngine () {
    m_thread->quit ();
    m_thread->wait ();
}

void SeriesEngine::requestSearch (QString name) {
    emit searchRequested (name);
}

void SeriesEngine::requestFullSerieInfo (QString slug, QString title, QString overview, QString banner, int tvdb_id) {
    emit fullSerieInfoRequested (slug, title, overview, banner, tvdb_id);
}

void SeriesEngine::onSearchResultsUpdated (QVariantList list) {
    qDebug () << "search results=" << list.length ();
    m_searchModel->clear ();
    foreach (QVariant variant, list) {
        QVariantMap values = variant.toMap ();
        qDebug () << "values=" << values;
        SeriesItem * item = SeriesItem::fromQtVariant (values);
        qDebug () << "item=" << item->get_banner () << item->get_slug () << item->get_title ();
        m_searchModel->append (item);
    }
    qDebug () << "search count=" << m_searchModel->count ();
}

void SeriesEngine::onSerieItemAdded (QString slug) {
    qDebug () << "serie item added=" << slug;
    if (!m_seriesModel->getByUid (slug)) {
        SeriesItem * item = new SeriesItem;
        item->update_slug (slug);
        m_seriesModel->append (item);
    }
}

void SeriesEngine::onSerieItemUpdated (QString slug, QVariantMap values) {
    qDebug () << "serie item updated=" << slug << values;
    QObject * object = m_seriesModel->getByUid (slug);
    if (object) {
        SeriesItem * item = qobject_cast<SeriesItem *> (object);
        if (item) {
            item->updateWithQtVariant (values);
        }
    }
}
