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

public:
    explicit SeriesEngine (QObject * parent = NULL);
    virtual ~SeriesEngine ();

signals:
    void searchRequested        (QString name);
    void fullSerieInfoRequested (QString slug, QString title, QString overview, QString banner, int tvdb_id);

public slots:
    void requestSearch        (QString name);
    void requestFullSerieInfo (QString slug, QString title, QString overview, QString banner, int tvdb_id);

private slots:
    void onSearchResultsUpdated (QVariantList list);
    void onSerieItemAdded       (QString      slug);
    void onSerieItemUpdated     (QString      slug, QVariantMap values);

private:
    SeriesWorker * m_worker;
    QThread      * m_thread;
};

#endif // SERIESENGINE_H
