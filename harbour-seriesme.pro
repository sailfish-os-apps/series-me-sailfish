# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-seriesme

MOC_DIR      = _moc
OBJECTS_DIR  = _obj
RCC_DIR      = _rcc

QT += core gui qml quick sql

CONFIG += sailfishapp

INCLUDEPATH += src/libQtQmlTricks

SOURCES += src/harbour-seriesme.cpp \
    src/SeriesWorker.cpp \
    src/SeriesEngine.cpp \
    src/SeriesItem.cpp \
    src/SeriesSeason.cpp \
    src/SeriesEpisode.cpp \
    src/libQtQmlTricks/qqmlvariantlistmodel.cpp \
    src/libQtQmlTricks/qqmlobjectlistmodel.cpp \
    src/libQtQmlTricks/qqmlhelpers.cpp \
    src/SeriesImageCacher.cpp

HEADERS += \
    src/SeriesWorker.h \
    src/SeriesEngine.h \
    src/SeriesItem.h \
    src/SeriesSeason.h \
    src/SeriesEpisode.h \
    src/libQtQmlTricks/qqmlvariantlistmodel_p.h \
    src/libQtQmlTricks/qqmlvariantlistmodel.h \
    src/libQtQmlTricks/qqmlobjectlistmodel_p.h \
    src/libQtQmlTricks/qqmlobjectlistmodel.h \
    src/libQtQmlTricks/qqmlmodels.h \
    src/libQtQmlTricks/qqmlhelpers.h \
    src/SeriesImageCacher.h


OTHER_FILES += qml/harbour-seriesme.qml \
    qml/cover/CoverPage.qml \
    qml/pages/SerieDetailPage.qml \
    qml/pages/SeriesListPage.qml \
    qml/pages/SerieSearchAndAddPage.qml \
    translations/harbour-seriesme.ts \
    rpm/harbour-seriesme.changes.in \
    rpm/harbour-seriesme.yaml \
    harbour-seriesme.desktop \
    harbour-seriesme.png \
    harbour-seriesme.svg \
    qml/components/CachedImage.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS +=

RESOURCES += \
    data.qrc


