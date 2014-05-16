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
            contentHeight: (bannerHeight * width / bannerWidth);
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

            Image {
                id: imgBanner;
                source: model ["banner"];
                opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                fillMode: Image.Stretch;
                asynchronous: true;
                anchors.fill: parent;
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
