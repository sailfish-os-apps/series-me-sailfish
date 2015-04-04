import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0
import "pages"
import "cover"

ApplicationWindow {
    cover: coverPage;
    initialPage: seriesListPage;

    readonly property int coverWidth     : 400;
    readonly property int coverHeight    : 576;
    readonly property int posterHeight   : 450;
    readonly property int posterWidth    : 300;
    readonly property int screenerWidth  : 400;
    readonly property int screenerHeight : 225;

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
