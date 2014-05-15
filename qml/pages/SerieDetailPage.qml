import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page;

    SilicaListView {
        id: view;
        model: engine.episodesModel;
        header: Column {
            id: layout;
            width: view.width;
            spacing: (Theme.paddingLarge * 2);

            PageHeader {
                id: pageTitle;
                title: (currentSerieItem ? currentSerieItem.title : "");
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
                    model: engine.seasonsModel;
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
                        onClicked: { currentSeasonIdx = model.seasonNumber; }

                        Image {
                            source: model.poster;
                            opacity: (parent.pressed ? 0.65 : 1.0);
                            anchors.fill: parent;
                        }
                        Label {
                            text: (model.seasonNumber ? qsTr ("S.%1%2").arg (model.seasonNumber < 10 ? "0" : "").arg (model.seasonNumber) : qsTr ("Specials"));
                            color: Theme [model.seasonNumber === currentSeasonIdx ? "highlightColor" : "secondaryHighlightColor"];
                            font.pixelSize: Theme [model.seasonNumber === currentSeasonIdx ? "fontSizeSmall" : "fontSizeExtraSmall"];
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
                text: (currentSerieItem ? currentSerieItem.overview : "");
                font.pixelSize: Theme.fontSizeMedium;
                visible: (currentSeasonIdx < 0);
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                horizontalAlignment: Text.AlignJustify;
                height: (contentHeight + Theme.paddingLarge);
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
                       text: (model.watched ? qsTr ("Watched") : qsTr ("Not watched yet"));
                    }
                    MenuItem {
                        text: qsTr ("Toggle 'watched' flag");
                        onClicked: { engine.requestToggleWatched (model.serieId, model.seasonNumber, model.episodeNumber, !model.watched); }
                    }
                }
            }

            Rectangle {
                color: (model.index % 2 ? "white" : "black");
                opacity: 0.05;
                anchors.fill: parent;
            }
            Image {
                id: imgScreener;
                source: model.screen;
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
                    text: (model.seasonNumber
                           ? qsTr ("<b>S%1E%2 : </b>").arg (model.seasonNumber).arg (model.episodeNumber)
                           : qsTr ("<b>Specials : </b>")) + model.title;
                    color: Theme.primaryColor;
                    font.pixelSize: Theme.fontSizeSmall;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                }
                Label {
                    text: model.overview;
                    color: Theme.secondaryColor;
                    font.pixelSize: Theme.fontSizeExtraSmall;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                }
            }
            GlassItem {
                color: Theme.highlightColor;
                visible: model.watched;
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
