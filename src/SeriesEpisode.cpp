
#include "SeriesEpisode.h"

SeriesEpisode::SeriesEpisode (QObject * parent) : QObject (parent) {
    m_serieId       = QStringLiteral ("");
    m_episodeId     = QStringLiteral ("");
    m_screen        = QStringLiteral ("");
    m_title         = QStringLiteral ("");
    m_overview      = QStringLiteral ("");
    m_seasonNumber  = -1;
    m_episodeNumber = -1;
    m_watched       = false;
}

SeriesEpisode * SeriesEpisode::fromQtVariant (const QVariantMap & values){
    SeriesEpisode * ret = new SeriesEpisode;
    ret->updateWithQtVariant (values);
    return ret;
}

void SeriesEpisode::updateWithQtVariant (const QVariantMap & values) {
    if (values.contains (QStringLiteral ("serieId"))) {
        update_serieId (values.value (QStringLiteral ("serieId")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("episodeId"))) {
        update_episodeId (values.value (QStringLiteral ("episodeId")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("screen"))) {
        update_screen (values.value (QStringLiteral ("screen")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("title"))) {
        update_title (values.value (QStringLiteral ("title")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("overview"))) {
        update_overview (values.value (QStringLiteral ("overview")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("seasonNumber"))) {
        update_seasonNumber (values.value (QStringLiteral ("seasonNumber")).value<int> ());
    }
    if (values.contains (QStringLiteral ("episodeNumber"))) {
        update_episodeNumber (values.value (QStringLiteral ("episodeNumber")).value<int> ());
    }
    if (values.contains (QStringLiteral ("watched"))) {
        update_watched (values.value (QStringLiteral ("watched")).value<bool> ());
    }
}

SeriesEpisode::~SeriesEpisode () { }
