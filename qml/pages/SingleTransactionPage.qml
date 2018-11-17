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
    property bool isPortfolio;

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
                title: transactionPage.isPortfolio ? qsTr("Portfolio Item") : qsTr("Transaction")
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
                    visible: !transactionPage.isPortfolio
                    text: transactionPage.isPortfolio ? "" : singleTransaction.volume.date.toLocaleDateString(Locale.ShortFormat)
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
                    text: transactionPage.isPortfolio ? singleTransaction.itemId : singleTransaction.details.otherPartyName
                    textFormat: Text.StyledText
                }
                Text {
                    id: transactionTextText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryHighlightColor
                    text: transactionPage.isPortfolio ? "" : singleTransaction.details.transactionText
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    visible: !transactionPage.isPortfolio
                }
                Text {
                    id: transactionPurposeText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    text: transactionPage.isPortfolio ? ( qsTr("<b>Amount: </b> %1").arg((singleTransaction.amountNegative ? "-" : "") + Number(singleTransaction.amount).toLocaleString(Qt.locale(), "f", 2)) + "<br>" + qsTr("<b>Price: </b> %1 %2").arg(Number(singleTransaction.price).toLocaleString(Qt.locale(), "f", 2)).arg(singleTransaction.priceCurrency) ) : singleTransaction.details.transactionPurpose
                    textFormat: Text.StyledText
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                }
                Text {
                    id: accountValueText
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.highlightColor
                    text: transactionPage.isPortfolio ? ((singleTransaction.valueNegative ? "-" : "") +  Number(singleTransaction.value).toLocaleString(Qt.locale(), "f", 2) + " " + singleTransaction.valueCurrency ) : (singleTransaction.volume.creditDebit === "D" ? "- " : "") + Number(singleTransaction.volume.value).toLocaleString(Qt.locale(), "f", 2)
                }
            }

            VerticalScrollDecorator {}

        }

    }

}

