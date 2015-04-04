import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0
import "../components"

Page {
    id: page;

    SilicaFlickable {
        id: view;
        contentHeight: (layout.height + layout.anchors.margins * 2);
        anchors.fill: parent;

        Column {
            id: layout;
            spacing: Theme.paddingMedium;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                margins: Theme.paddingSmall;
            }

            PageHeader {
                title: qsTr ("Add a new serie");
            }
            SearchField {
                id: inputSearch;
                label: placeholderText;
                placeholderText: qsTr ("Search by name");
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                Keys.onReturnPressed: {
                    focus = false;
                    engine.requestSearch (text);
                }
            }
            Grid {
                id: grid;
                columns: 2;
                rowSpacing: layout.spacing;
                columnSpacing: layout.spacing;
                spacing: layout.spacing;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                readonly property real itemWidth  : ((width - (spacing * (columns -1))) / columns);
                readonly property real itemHeight : (posterHeight * itemWidth / posterWidth);

                Repeater {
                    id: repeater;
                    model: engine.searchModel;
                    delegate: Image {
                        id: imgBanner;
                        width: grid.itemWidth;
                        height: grid.itemHeight;
                        source: model ["banner"];
                        opacity: (clicker.pressed ? 0.85 : 1.0);
                        fillMode: Image.Stretch;
                        asynchronous: true;

                        Text {
                            z: -1;
                            text: model ["title"];
                            color: Theme.secondaryHighlightColor;
                            fontSizeMode: Text.Fit;
                            verticalAlignment: Text.AlignVCenter;
                            horizontalAlignment: Text.AlignHCenter;
                            font.pixelSize: Theme.fontSizeMedium;
                            anchors {
                                fill: parent;
                                margins: Theme.paddingLarge;
                            }
                        }
                        MouseArea {
                            id: clicker;
                            anchors.fill: parent;
                            onClicked: {
                                engine.requestFullSerieInfo (model ["serieId"],
                                                             model ["title"],
                                                             model ["overview"],
                                                             model ["banner"]);
                                pageStack.navigateBack ();
                            }
                        }
                    }
                }
            }
            Label {
                text: qsTr ("%1 results").arg (repeater.count);
                horizontalAlignment: Text.AlignHCenter;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }
        }
        VerticalScrollDecorator { }
    }
}
