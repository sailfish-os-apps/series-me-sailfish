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

CONFIG += sailfishapp

SOURCES += src/harbour-seriesme.cpp

OTHER_FILES += qml/harbour-seriesme.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-seriesme.changes.in \
    rpm/harbour-seriesme.yaml \
    harbour-seriesme.desktop \
    qml/pages/SerieDetailPage.qml \
    qml/pages/SeriesListPage.qml \
    harbour-seriesme.png \
    translations/harbour-seriesme.ts \
    qml/pages/SerieSearchAndAddPage.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS +=

