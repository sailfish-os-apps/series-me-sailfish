
#include "SeriesItem.h"

SeriesItem::SeriesItem (QObject * parent) : QObject (parent) {
    m_serieId  = QStringLiteral ("");
    m_title    = QStringLiteral ("{{ serie title }}");
    m_banner   = QStringLiteral ("");
    m_overview = QStringLiteral ("{{ serie overview }}");
    m_fetched  = QDateTime ();
}

SeriesItem * SeriesItem::fromQtVariant (const QVariantMap & values) {
    SeriesItem * ret = new SeriesItem;
    ret->updateWithQtVariant (values);
    return ret;
}

void SeriesItem::updateWithQtVariant (const QVariantMap & values) {
    if (values.contains (QStringLiteral ("serieId"))) {
        update_serieId (values.value (QStringLiteral ("serieId")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("title"))) {
        update_title (values.value (QStringLiteral ("title")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("banner"))) {
        update_banner (values.value (QStringLiteral ("banner")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("overview"))) {
        update_overview (values.value (QStringLiteral ("overview")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("fetched"))) {
        update_fetched (values.value (QStringLiteral ("fetched")).value<QDateTime> ());
    }
}

SeriesItem::~SeriesItem () { }
