import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

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
                        onClicked: {
                            currentEpisodeId = "";
                            currentSeasonIdx = model.seasonNumber;
                        }

                        CachedImage {
                            source: model.poster;
                            opacity: (parent.pressed ? 0.85 : 1.0);
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
            contentHeight: (isCurrent
                            ? lblSummary.y + lblSummary.height + Theme.paddingMedium
                            : imgScreener.height + Theme.paddingMedium * 2);
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
            onClicked: { currentEpisodeId = (currentEpisodeId !== model.episodeId ? model.episodeId : ""); }

            property bool isCurrent : (model.episodeId === currentEpisodeId);

            Rectangle {
                color: (model.index % 2 ? "white" : "black");
                opacity: 0.05;
                anchors.fill: parent;
            }
            CachedImage {
                id: imgScreener;
                source: model.screen;
                opacity: (itemEpisode.highlighted ? 0.85 : 1.0);
                width: Theme.itemSizeLarge;
                height: (screenerHeight * width / screenerWidth);
                anchors {
                    top: parent.top;
                    left: parent.left;
                    margins: Theme.paddingMedium;
                }
            }
            Label {
                text: (model.seasonNumber
                       ? qsTr ("<b>S%1E%2 : </b>").arg (model.seasonNumber).arg (model.episodeNumber)
                       : qsTr ("<b>Specials : </b>")) + model.title;
                color: Theme.primaryColor;
                font.pixelSize: Theme.fontSizeSmall;
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                anchors {
                    left: imgScreener.right;
                    right: parent.right;
                    leftMargin: Theme.paddingMedium;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: imgScreener.verticalCenter;
                }
            }
            Label {
                id: lblSummary;
                text: model.overview;
                color: Theme.secondaryColor;
                visible: isCurrent;
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                horizontalAlignment: Text.AlignJustify;
                font.pixelSize: Theme.fontSizeExtraSmall;
                anchors {
                    top: imgScreener.bottom;
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingMedium;
                }
            }
            GlassItem {
                color: Theme.highlightColor;
                visible: model.watched;
                anchors {
                    verticalCenter: imgScreener.verticalCenter;
                    horizontalCenter: parent.right;
                }
            }
        }
        anchors.fill: parent;

        PullDownMenu {
            MenuItem {
                text: qsTr ("Remove this serie");
                enabled: false; // FIXME
            }
            MenuItem {
                text: qsTr ("Update metadata");
                enabled: false; // FIXME
            }
        }
        PushUpMenu {
            MenuItem {
                text: qsTr ("Mark this season as watched");
                enabled: false; // FIXME
            }
        }
        VerticalScrollDecorator { }
    }
}
