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
    void serieItemAdded       (QString      slug);
    void serieItemUpdated     (QString      slug, QVariantMap values);

public slots:
    void initialize       ();
    void searchForSerie   (QString name);
    void getFullSerieInfo (QString slug, QString title, QString overview, QString banner, int tvdb_id);

private slots:
    void onSearchReply  ();

private:
    QSqlDatabase            m_db;
    QNetworkAccessManager * m_nam;
};

#endif // SERIESWORKER_H
