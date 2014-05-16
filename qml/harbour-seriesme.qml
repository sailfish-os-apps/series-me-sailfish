import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0
import "pages"
import "cover"

ApplicationWindow {
    cover: coverPage;
    initialPage: seriesListPage;
    onCurrentSeasonIdxChanged: {
        if (currentSeasonIdx >= 0) {
            engine.requestLoadEpisodes (currentSerieSlug, currentSeasonIdx);
        }
    }
    onCurrentSerieSlugChanged: {
        if (currentSerieSlug !== "") {
            currentSerieItem = engine.seriesModel.getByUid (currentSerieSlug);
            engine.requestLoadSeasons (currentSerieSlug);
        }
    }

    property int coverWidth     : 400;
    property int coverHeight    : 576;
    property int bannerHeight   : 140;
    property int bannerWidth    : 758;
    property int screenerWidth  : 400;
    property int screenerHeight : 225;

    property int    currentSeasonIdx : -1;
    property string currentSerieSlug : "";
    property string currentEpisodeId : "";

    property SeriesItem currentSerieItem : null;

    SeriesEngine {
        id: engine;
    }
    Component {
        id: coverPage;

        CoverPage { }
    }
    Component {
        id: seriesListPage;

        SeriesListPage { }
    }
    Component {
        id: serieDetailPage;

        SerieDetailPage { }
    }
    Component {
        id: serieSearchAndAddPage;

        SerieSearchAndAddPage { }
    }
}
