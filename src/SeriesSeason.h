#ifndef SERIESSEASON_H
#define SERIESSEASON_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QVariant>
#include <QQmlHelpers>

class SeriesSeason : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QString, serieId)      // slug
    QML_READONLY_PROPERTY (QString, seasonId)     // slug_SXX
    QML_READONLY_PROPERTY (QString, poster)       // url
    QML_READONLY_PROPERTY (int,     seasonNumber) // XX
    QML_READONLY_PROPERTY (int,     episodeCount) // N

public:
    explicit SeriesSeason (QObject * parent = NULL);
    static   SeriesSeason * fromQtVariant (const QVariantMap & values);
    virtual ~SeriesSeason ();

    void updateWithQtVariant (const QVariantMap & values);
};

#endif // SERIESSEASON_H
