#ifndef SERIESEPISODE_H
#define SERIESEPISODE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QVariant>
#include <QQmlHelpers>

class SeriesEpisode : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QString, serieId)       // slug
    QML_READONLY_PROPERTY (QString, episodeId)     // slug_SXX_EYY
    QML_READONLY_PROPERTY (QString, screen)        // url
    QML_READONLY_PROPERTY (QString, title)         // title
    QML_READONLY_PROPERTY (QString, overview)      // overview
    QML_READONLY_PROPERTY (int,     seasonNumber)  // XX
    QML_READONLY_PROPERTY (int,     episodeNumber) // YY
    QML_READONLY_PROPERTY (bool,    watched)       // true|false

public:
    explicit SeriesEpisode (QObject * parent = NULL);
    static   SeriesEpisode * fromQtVariant (const QVariantMap & values);
    virtual ~SeriesEpisode ();

    void updateWithQtVariant (const QVariantMap & values);
};

#endif // SERIESEPISODE_H
