#ifndef SERIESITEM_H
#define SERIESITEM_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QVariant>
#include <QQmlHelpers>

class SeriesItem : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QString,   serieId)
    QML_READONLY_PROPERTY (QString,   title)
    QML_READONLY_PROPERTY (QString,   banner)
    QML_READONLY_PROPERTY (QString,   overview)
    QML_READONLY_PROPERTY (QDateTime, fetched)

public:
    explicit SeriesItem (QObject * parent = NULL);
    static   SeriesItem * fromQtVariant (const QVariantMap & values);
    virtual ~SeriesItem ();

    void updateWithQtVariant (const QVariantMap & values);
};

#endif // SERIESITEM_H
