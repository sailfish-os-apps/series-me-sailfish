import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page;

    property int    currentSeasonIdx : -1;
    property string imgPrefix        : "";
    property string imgSuffix        : "";

    SilicaListView {
        id: view;
        model: (currentSeasonIdx > -1 ? 25 : 0);
        header: Column {
            id: layout;
            width: view.width;
            spacing: (Theme.paddingLarge * 2);

            PageHeader {
                id: pageTitle;
                title: qsTr ("{{ Serie title }}"); // TODO
            }
            Item {
                id: containerSeasons;
                height: (page.height * 0.20);
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                Rectangle {
                    color: "white";
                    opacity: 0.15;
                    anchors.fill: parent;
                }
                SilicaListView {
                    id: viewSeasons;
                    model: 10;
                    spacing: Theme.paddingMedium;
                    orientation: ListView.Horizontal;
                    header: Item { width: viewSeasons.spacing; }
                    footer: Item { width: viewSeasons.spacing; }
                    delegate: MouseArea {
                        id: delegate
                        width: (coverWidth * height / coverHeight);
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                            margins: Theme.paddingMedium;
                        }
                        onClicked: { currentSeasonIdx = model.index; }

                        Image {
                            source: "http://slurm.trakt.us/images/seasons/%1-%2.%3.jpg".arg (imgPrefix).arg (model.index).arg (imgSuffix);
                            opacity: (parent.pressed ? 0.65 : 1.0);
                            anchors.fill: parent;
                        }
                        Label {
                            text: (model.index ? qsTr ("S.%1%2").arg (model.index < 10 ? "0" : "").arg (model.index) : qsTr ("Specials"));
                            color: Theme [model.index === currentSeasonIdx ? "highlightColor" : "secondaryHighlightColor"];
                            font.pixelSize: Theme [model.index === currentSeasonIdx ? "fontSizeSmall" : "fontSizeExtraSmall"];
                            anchors {
                                bottom: parent.top;
                                margins: (Theme.paddingMedium + Theme.paddingSmall);
                                horizontalCenter: parent.horizontalCenter;
                            }
                        }
                    }
                    anchors.fill: parent;

                    HorizontalScrollDecorator { }
                }
            }
            Label {
                text: qsTr ("{{ Serie summary }}");
                font.pixelSize: Theme.fontSizeLarge;
                visible: (currentSeasonIdx < 0);
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingLarge;
                }
            }
        }
        delegate: ListItem {
            id: itemEpisode;
            contentHeight: Math.max (layoutText.height, imgScreener.height) + Theme.paddingMedium * 2;
            menu: Component {
                ContextMenu {
                    MenuLabel {
                       text: (itemEpisode.watched ? qsTr ("Watched") : qsTr ("Not watched yet"));
                    }
                    MenuItem {
                        text: qsTr ("Toggle 'watched' flag");
                        onClicked: { itemEpisode.watched = !itemEpisode.watched; }
                    }
                }
            }

            property bool watched : (model.index < 6); // TODO

            Rectangle {
                color: (model.index % 2 ? "white" : "black");
                opacity: 0.05;
                anchors.fill: parent;
            }
            Image {
                id: imgScreener;
                source: "http://slurm.trakt.us/images/episodes/%1-%2-%3.%4.jpg".arg (imgPrefix).arg (currentSeasonIdx).arg (model.index +1).arg (imgSuffix);
                width: Theme.itemSizeLarge;
                height: (screenerHeight * width / screenerWidth);
                anchors {
                    top: parent.top;
                    left: parent.left;
                    margins: Theme.paddingMedium;
                }
            }
            Column {
                id: layoutText;
                anchors {
                    top: parent.top;
                    left: imgScreener.right;
                    right: parent.right;
                    margins: Theme.paddingMedium;
                    rightMargin: Theme.paddingLarge;
                }

                Label {
                    text: (currentSeasonIdx ? qsTr ("<b>S%1E%2 : </b>").arg (currentSeasonIdx).arg (model.index) : qsTr ("<b>Specials : </b>")) + "{{ Episode title }}"; // TODO
                    color: Theme.primaryColor;
                    font.pixelSize: Theme.fontSizeSmall;
                }
                Label {
                    text: qsTr ("{{ Episode summary }}"); // TODO
                    color: Theme.secondaryColor;
                    font.pixelSize: Theme.fontSizeExtraSmall;
                }
            }
            GlassItem {
                color: Theme.highlightColor;
                visible: watched;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    horizontalCenter: parent.right;
                }
            }
        }
        anchors.fill: parent;

        PullDownMenu {
            MenuItem {
                text: qsTr ("Remove this serie");
            }
            MenuItem {
                text: qsTr ("Update metadata");
            }
        }
        PushUpMenu {
            MenuItem {
                text: qsTr ("Mark this season as watched");
            }
        }
        VerticalScrollDecorator { }
    }
}
