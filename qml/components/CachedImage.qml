import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0

Item {
    id: self;
    width: implicitWidth;
    height: implicitHeight;
    implicitWidth: img.sourceSize.width;
    implicitHeight: img.sourceSize.height;

    property alias source : cacher.remoteSource;

    Image {
        id: img;
        source: cacher.localSource;
        fillMode: Image.PreserveAspectFit;
        asynchronous: true;
        verticalAlignment: Image.AlignVCenter;
        horizontalAlignment: Image.AlignHCenter;
        anchors.fill: parent;
    }
    BusyIndicator {
        id: indicator;
        visible: running;
        running: (cacher.localSource === "" || img.status !== Image.Ready);
        size: BusyIndicatorSize.Small;
        anchors.centerIn: parent;
    }
    SeriesImageCacher { id: cacher; }
}
