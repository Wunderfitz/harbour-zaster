/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster.

    Zaster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: transactionPage
    allowedOrientations: Orientation.All

    property variant singleTransaction;

    SilicaFlickable {

        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: singleTransactionColumn.height

        Column {
            id: singleTransactionColumn
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: singleTransactionHeader
                title: qsTr("Transaction")
            }
            Column {
                id: transactionColumn
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium
                Text {
                    id: transactionDateText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryColor
                    text: singleTransaction.volume.date.toLocaleDateString(Locale.ShortFormat)
                }
                Text {
                    id: otherPartyNameText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeSmall
                    font.bold: true
                    color: Theme.primaryColor
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    wrapMode: Text.Wrap
                    visible: text ? true : false
                    text: singleTransaction.details.otherPartyName
                }
                Text {
                    id: transactionTextText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryHighlightColor
                    text: singleTransaction.details.transactionText
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
                Text {
                    id: transactionPurposeText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    text: singleTransaction.details.transactionPurpose
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                }
                Text {
                    id: accountValueText
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.highlightColor
                    text: (singleTransaction.volume.creditDebit === "D" ? "- " : "") + Number(singleTransaction.volume.value).toLocaleString(Qt.locale(), "f", 2)
                }
            }

            VerticalScrollDecorator {}

        }

    }

}

