import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    SilicaListView {
        id: view;
        spacing: 1;
        model: ["198.53", "71.7", "92.8", "261.2", "329.3", "59.7", "68.7"];
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
            anchors {
                left: parent.left;
                right: parent.right;
            }
            onClicked: {
                var tmp = model.modelData.split (".");
                var imgPrefix = (tmp [0] || '');
                var imgSuffix = (tmp [1] || '');
                pageStack.push (serieDetailPage, {
                                    "imgPrefix" : imgPrefix,
                                    "imgSuffix" : imgSuffix
                                });
            }

            Image {
                id: imgBanner;
                opacity: (itemSerie.highlighted ? 0.85 : 1.0);
                source: "http://slurm.trakt.us/images/banners/%1.jpg".arg (model.modelData);
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
