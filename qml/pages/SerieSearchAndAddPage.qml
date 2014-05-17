import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0
import "../components"

Page {
    id: page;

    SilicaListView {
        id: view;
        model: engine.searchModel;
        header: Column {
            width: view.width;
            spacing: Theme.paddingMedium;

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
        }
        delegate: ListItem {
            id: itemSerie;
            contentHeight: (layout.height + layout.anchors.margins * 2);
            anchors {
                left: parent.left;
                right: parent.right;
            }
            onClicked: {
                engine.requestFullSerieInfo (model ["serieId"],
                                             model ["title"],
                                             model ["overview"],
                                             model ["banner"]);
                pageStack.navigateBack ();
            }

            Rectangle {
                color: "white";
                opacity: 0.15;
                anchors {
                    fill: parent;
                    margins: Theme.paddingSmall;
                }
            }
            Column {
                id: layout;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingLarge;
                }

                Label {
                    text: model ["title"];
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                }
                Image {
                    id: imgBanner;
                    source: model ["banner"];
                    opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                    fillMode: Image.Stretch;
                    asynchronous: true;
                    height: (bannerHeight * width / bannerWidth);
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                }
            }
        }
        footer: Label {
            text: qsTr ("%1 results").arg (view.count);
            width: view.width;
            horizontalAlignment: Text.AlignHCenter;
        }
        anchors.fill: parent;

        VerticalScrollDecorator {}
    }
}
