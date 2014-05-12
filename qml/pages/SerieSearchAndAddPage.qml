import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.seriesme.myPrivateImports 1.0

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
                placeholderText: "Search by name";
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
            contentHeight: (bannerHeight * width / bannerWidth);
            anchors {
                left: parent.left;
                right: parent.right;
            }
            onClicked: {
                engine.requestFullSerieInfo (model ["slug"],
                                             model ["title"],
                                             model ["overview"],
                                             model ["banner"],
                                             model ["tvdb_id"]);
                pageStack.navigateBack ();
            }

            Image {
                id: imgBanner;
                opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                source: model ["banner"];
                fillMode: Image.Stretch;
                asynchronous: true;
                anchors.fill: parent;
            }
            BusyIndicator {
                visible: running;
                running: (imgBanner.status !== Image.Ready);
                anchors.centerIn: parent;
            }
        }
        footer: Label {
            text: qsTr ("%1 results").arg (view.count);
            horizontalAlignment: Text.AlignHCenter;
            anchors {
                left: parent.left;
                right: parent.right;
            }
        }
        anchors.fill: parent;

        VerticalScrollDecorator {}
    }
}
