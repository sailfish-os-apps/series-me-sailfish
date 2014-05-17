import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    SilicaListView {
        id: view;
        spacing: 1;
        model: engine.seriesModel;
        header: Column {
            id: column;
            width: view.width;
            spacing: Theme.paddingLarge;

            PageHeader {
                title: qsTr ("Series'me");
            }
            Label {
                text: qsTr ("Track your watching status of TV series...");
                color: Theme.secondaryHighlightColor;
                font.pixelSize: Theme.fontSizeSmall;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingLarge;
                }
            }
            Item {
                width: Theme.paddingLarge;
                height: Theme.paddingLarge;
            }
        }
        delegate: ListItem {
            id: itemSerie;
            contentHeight: (bannerHeight * width / bannerWidth);
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr ("Remove this serie");
                        onClicked: { remove (); }
                    }
                }
            }
            anchors {
                left: parent.left;
                right: parent.right;
            }
            onClicked: {
                engine.currentSerieId = model.serieId;
                pageStack.push (serieDetailPage, { });
            }

            function remove () {
                var tmp = model.serieId;
                remorse.execute (itemSerie,
                                 qsTr ("Deleting"),
                                 function () {
                                     engine.requestRemoveSerie (tmp);
                                 });
            }

            RemorseItem { id: remorse; }
            CachedImage {
                id: imgBanner;
                opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                source: model.banner;
                anchors.fill: parent;
            }
        }
        anchors.fill: parent;

        PullDownMenu {
            MenuItem {
                text: qsTr ("Add a new serie");
                onClicked: { pageStack.push (serieSearchAndAddPage); }
            }
        }
        VerticalScrollDecorator {}
    }
}
