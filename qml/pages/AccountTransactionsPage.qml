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
    id: transactionsPage
    allowedOrientations: Orientation.All

    property string accountId;
    property bool isPortfolio: false

    Component.onCompleted: {
        finTsDialog.dialogInitialization();
        loadingColumn.visible = true;
    }

    Connections {
        target: finTsDialog
        onDialogInitializationCompleted: {
            if (finTsDialog.canRetrieveTransactions(transactionsPage.accountId)) {
                transactionsHeader.title = qsTr("Transactions");
                finTsDialog.accountTransactions(transactionsPage.accountId);
            } else if (finTsDialog.canRetrievePortfolioInfo(transactionsPage.accountId)) {
                transactionsHeader.title = qsTr("Portfolio");
                transactionsPage.isPortfolio = true;
                finTsDialog.portfolioInfo(transactionsPage.accountId);
            }
        }
        onDialogInitializationFailed: {
            loadingColumn.visible = false;
            errorColumn.retryPossible = true;
            errorColumn.visible = true;
            errorInfoLabel.text = qsTr("Unable to connect to your bank. Please ensure that your internet connection works properly and try again.");
        }
        onAccountTransactionsCompleted: {
            finTsDialog.closeDialog();
            loadingColumn.visible = false;
            transactionsColumn.visible = true;
            transactionsListView.model = accountTransactions;
        }
        onAccountTransactionsFailed: {
            loadingColumn.visible = false;
            errorColumn.retryPossible = true;
            errorColumn.visible = true;
            errorInfoLabel.text = qsTr("Unable to connect to your bank. Please ensure that your internet connection works properly and try again.");
        }
        onPortfolioInfoCompleted: {
            finTsDialog.closeDialog();
            loadingColumn.visible = false;
            transactionsColumn.visible = true;
            transactionsListView.model = portfolioItems;
        }
        onPortfolioInfoFailed: {
            loadingColumn.visible = false;
            errorColumn.retryPossible = true;
            errorColumn.visible = true;
            errorInfoLabel.text = qsTr("Unable to connect to your bank. Please ensure that your internet connection works properly and try again.");
        }
        onDialogEndCompleted: {
            console.log("Dialog successfully terminated.");
        }
        onDialogEndFailed: {
            console.log("Error terminating dialog.");
        }
    }

    SilicaFlickable {

        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: transactionsColumn.visible ? transactionsColumn.height : parent.height

        Column {
            id: loadingColumn
            width: parent.width
            height: loadingLabel.height + loadingBusyIndicator.height + zasterLoadingImage.height + ( 2 * Theme.paddingMedium )
            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1 : 0
            visible: false

            Image {
                id: zasterLoadingImage
                source: "../../images/zaster.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                id: loadingLabel
                text: qsTr("Retrieving information...")
            }

            BusyIndicator {
                id: loadingBusyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                running: loadingColumn.visible
                size: BusyIndicatorSize.Large
            }
        }

        Column {

            property bool retryPossible: false

            id: errorColumn
            height: errorInfoLabel.height + zasterErrorImage.height + errorOkButton.height + ( 3 * Theme.paddingMedium )
            width: parent.width
            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1 : 0
            visible: false

            Image {
                id: zasterErrorImage
                source: "../../images/zaster.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                id: errorInfoLabel
                font.pixelSize: Theme.fontSizeLarge
                text: ""
            }

            Button {
                id: errorOkButton
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {

                    if (errorColumn.retryPossible) {
                        errorColumn.visible = false;
                        finTsDialog.dialogInitialization();
                        loadingColumn.visible = true;
                    } else {
                        errorColumn.visible = false;
                        pageStack.pop();
                    }

                }
            }
        }

        Column {
            id: transactionsColumn
            width: parent.width
            spacing: Theme.paddingMedium

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1 : 0
            visible: false

            PageHeader {
                id: transactionsHeader
            }

            SilicaListView {

                id: transactionsListView

                height: transactionsPage.height - transactionsHeader.height - Theme.paddingMedium
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right

                clip: true

                delegate: ListItem {
                    contentHeight: resultItem.height + ( 2 * Theme.paddingMedium )
                    contentWidth: parent.width

                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("SingleTransactionPage.qml"), {"singleTransaction": modelData })
                    }

                    Item {
                        id: resultItem
                        width: parent.width
                        height: resultRow.height + transactionSeparator.height
                        y: Theme.paddingMedium

                        Row {
                            id: resultRow
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            spacing: Theme.paddingMedium
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            Column {
                                width: parent.width / 3 * 2 - Theme.paddingSmall
                                Text {
                                    id: transactionDateText
                                    width: parent.width
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.secondaryColor
                                    visible: !transactionsPage.isPortfolio
                                    text: transactionsPage.isPortfolio ? "" : modelData.volume.date.toLocaleDateString(Locale.ShortFormat)
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
                                    text: transactionsPage.isPortfolio ? modelData.itemId : modelData.details.otherPartyName
                                    textFormat: Text.StyledText
                                }
                                Text {
                                    id: transactionTextText
                                    width: parent.width
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.secondaryHighlightColor
                                    text: transactionsPage.isPortfolio ? "" : modelData.details.transactionText
                                    textFormat: Text.StyledText
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    visible: !transactionsPage.isPortfolio
                                }
                                Text {
                                    id: transactionPurposeText
                                    width: parent.width
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    color: Theme.primaryColor
                                    text: transactionsPage.isPortfolio ? ( qsTr("<b>Amount: </b> %1").arg((modelData.amountNegative ? "-" : "") + Number(modelData.amount).toLocaleString(Qt.locale(), "f", 2)) + "<br>" + qsTr("<b>Price: </b> %1 %2").arg(Number(modelData.price).toLocaleString(Qt.locale(), "f", 2)).arg(modelData.priceCurrency) ) : modelData.details.transactionPurpose
                                    wrapMode: Text.Wrap
                                    elide: Text.ElideRight
                                    maximumLineCount: 4
                                }
                            }
                            Text {
                                id: accountValueText
                                width: parent.width / 3 * 1 - Theme.paddingSmall
                                height: parent.height
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: Theme.fontSizeMedium
                                color: Theme.highlightColor
                                text: transactionsPage.isPortfolio ? ((modelData.valueNegative ? "-" : "") +  Number(modelData.value).toLocaleString(Qt.locale(), "f", 2) + " " + modelData.valueCurrency ) : (modelData.volume.creditDebit === "D" ? "- " : "") + Number(modelData.volume.value).toLocaleString(Qt.locale(), "f", 2)
                            }
                        }

                        Separator {
                            id: transactionSeparator
                            anchors.top : resultRow.bottom
                            anchors.topMargin: Theme.paddingMedium

                            width: parent.width
                            color: Theme.primaryColor
                            horizontalAlignment: Qt.AlignHCenter
                        }
                    }

                }

                VerticalScrollDecorator {}

            }


        }

    }

}

