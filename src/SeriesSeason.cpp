
#include "SeriesSeason.h"

SeriesSeason::SeriesSeason (QObject * parent) : QObject (parent) {
    m_serieId      = QStringLiteral ("");
    m_seasonId     = QStringLiteral ("");
    m_poster       = QStringLiteral ("");
    m_seasonNumber = -1;
    m_episodeCount = 0;
}

SeriesSeason * SeriesSeason::fromQtVariant (const QVariantMap & values){
    SeriesSeason * ret = new SeriesSeason;
    ret->updateWithQtVariant (values);
    return ret;
}

void SeriesSeason::updateWithQtVariant (const QVariantMap & values) {
    if (values.contains (QStringLiteral ("serieId"))) {
        update_serieId (values.value (QStringLiteral ("serieId")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("seasonId"))) {
        update_seasonId (values.value (QStringLiteral ("seasonId")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("poster"))) {
        update_poster (values.value (QStringLiteral ("poster")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("seasonNumber"))) {
        update_seasonNumber (values.value (QStringLiteral ("seasonNumber")).value<int> ());
    }
    if (values.contains (QStringLiteral ("episodeCount"))) {
        update_episodeCount (values.value (QStringLiteral ("episodeCount")).value<int> ());
    }
}

SeriesSeason::~SeriesSeason () { }

