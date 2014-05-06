import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import "cover"

ApplicationWindow {
    cover: coverPage;
    initialPage: seriesListPage;

    property int coverWidth     : 400;
    property int coverHeight    : 576;
    property int bannerHeight   : 140;
    property int bannerWidth    : 758;
    property int screenerWidth  : 400;
    property int screenerHeight : 225;

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
