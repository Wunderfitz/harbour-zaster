/*
    Copyright (C) 2018-19 Sebastian J. Wolf

    This file is part of Zaster Banker.

    Zaster Banker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster Banker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster Banker. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: institutesSearchPage

    allowedOrientations: Orientation.All

    SilicaFlickable {

        anchors.fill: parent
        contentHeight: parent.height
        contentWidth: parent.width

        Column {
            id: searchColumn

            Behavior on opacity { NumberAnimation {} }
            width: parent.width
            spacing: Theme.paddingSmall

            Timer {
                id: searchTimer
                interval: 800
                running: false
                repeat: false
                onTriggered: {
                    finTsDialog.searchInstitute(searchField.text);
                }
            }

            Connections {
                target: finTsDialog
                onInstitutesSearchCompleted: {
                    searchListView.model = resultList;
                    ( searchListView.count === 0 && searchField.text !== "" ) ?  noResultsColumn.visible = true :  noResultsColumn.visible = false;
                }
            }

            PageHeader {
                id: searchHeader
                title: qsTr("Welcome to Zaster Banker")
            }

            SearchField {
                id: searchField
                width: parent.width
                placeholderText: qsTr("Find your bank...")
                focus: true

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: focus = false

                onTextChanged: {
                    searchTimer.stop()
                    searchTimer.start()
                }
            }

            Column {
                height: institutesSearchPage.height - searchHeader.height - searchField.height - abortButton.height - 3 * Theme.paddingSmall - separatorLabel.height
                width: parent.width

                id: noResultsColumn
                Behavior on opacity { NumberAnimation {} }
                opacity: visible ? 1 : 0
                visible: false

                Label {
                    id: noResultsLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("No results found")
                    color: Theme.secondaryColor
                }
            }

            SilicaListView {

                id: searchListView

                height: institutesSearchPage.height - searchHeader.height - searchField.height - abortButton.height - 3 * Theme.paddingSmall - separatorLabel.height
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right
                opacity: ( searchListView.count === 0 && searchField.text !== "" ) ? 0 : 1
                visible: ( searchListView.count === 0 && searchField.text !== "" ) ? false : true

                Behavior on opacity { NumberAnimation {} }

                clip: true

                delegate: ListItem {
                    contentHeight: resultColumn.height + Theme.paddingMedium
                    contentWidth: parent.width

                    onClicked: {
                        console.log("Selected: " + modelData.name + ", " + modelData.blz + ", " + modelData.url);
                        finTsDialog.initializeParameters();
                        finTsDialog.setBankData(modelData.blz, modelData.name, modelData.url);
                        pageStack.push(Qt.resolvedUrl("CredentialsPage.qml"));
                    }

                    Column {
                        id: resultColumn
                        width: parent.width - ( 2 * Theme.horizontalPageMargin )
                        height: bankNameText.height + bankAdditionalInfoRow.height + Theme.paddingMedium
                        spacing: Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            id: bankNameText
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.primaryColor
                            text: modelData.name
                            textFormat: Text.StyledText
                            elide: Text.ElideRight
                            maximumLineCount: 1
                            width: parent.width
                            height: Theme.fontSizeMedium
                        }
                        Row {
                            id: bankAdditionalInfoRow
                            height: Theme.fontSizeMedium
                            width: parent.width
                            Text {
                                id: bankBlzText
                                width: parent.width / 2
                                font.pixelSize: Theme.fontSizeExtraSmall
                                color: Theme.secondaryColor
                                text: qsTr("Bank ID: %1").arg(modelData.blz)
                                textFormat: Text.StyledText
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                            Text {
                                id: bankLocationText
                                width: parent.width / 2
                                font.pixelSize: Theme.fontSizeExtraSmall
                                color: Theme.secondaryColor
                                text: modelData.location
                                textFormat: Text.StyledText
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                        }
                    }

                }

                VerticalScrollDecorator {}

            }

            Button {
                id: abortButton
                text: qsTr("Abort")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    finTsAccounts.removeCurrentAccount();
                    pageStack.clear();
                    pageStack.push(finTsDialog.isInitialized() ? Qt.resolvedUrl("OverviewPage.qml") : Qt.resolvedUrl("InstitutesSearchPage.qml"));
                }
            }

            Label {
                id: separatorLabel
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

        }

    }

}
