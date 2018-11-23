/*
    Copyright (C) 2018 Sebastian J. Wolf

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


CoverBackground {

    id: coverBackground

    property bool balancesRetrieved: false;

    Connections {
        target: finTsBalances
        onBalancesRetrieved: {
            accountsListView.model = accountBalances;
            bankNameText.text = finTsDialog.getBankName();
            balancesRetrieved = true;
        }
    }

    Image {
        source: "../../images/background.png"
        anchors {
            verticalCenter: parent.verticalCenter

            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium

            right: parent.right
            rightMargin: Theme.paddingMedium
        }

        fillMode: Image.PreserveAspectFit
        opacity: 0.2
    }

    Column {
        id: coverColumn

        visible: coverBackground.balancesRetrieved
        spacing: Theme.paddingMedium

        anchors {
            top: parent.top
            topMargin: Theme.paddingMedium
            left: parent.left
            //leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.paddingMedium
            bottom: parent.bottom
        }

        Text {
            id: bankNameText
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            color: Theme.primaryColor
            font.pixelSize: Theme.fontSizeMedium
            font.bold: true
            textFormat: Text.StyledText
            elide: Text.ElideRight
            wrapMode: Text.Wrap
        }

        SilicaListView {

            id: accountsListView

            height: parent.height - bankNameText.height - Theme.paddingMedium
            width: parent.width
            anchors.left: parent.left
            anchors.right: parent.right

            clip: true

            delegate: ListItem {
                contentHeight: resultItem.height + Theme.paddingMedium
                contentWidth: parent.width

                opacity: index < 3 ? 1.0 - index * 0.3 : 0.0

                Item {
                    id: resultItem
                    width: parent.width
                    height: resultColumn.height + Theme.paddingMedium

                    Column {
                        id: resultColumn
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            id: accountIdText
                            width: parent.width
                            font.pixelSize: Theme.fontSizeSmall
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            color: Theme.primaryColor
                            text: modelData.accountId
                        }
                        Text {
                            id: accountValueText
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: Theme.fontSizeSmall
                            font.bold: true
                            color: Theme.highlightColor
                            text: (modelData.creditDebit === "D" ? "- " : "") + Number(modelData.value).toLocaleString(Qt.locale(), "f", 2) + " " + modelData.currency
                        }
                    }
                }

            }

            VerticalScrollDecorator {}

        }


    }

}


