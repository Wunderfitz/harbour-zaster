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

    Component.onCompleted: {
        finTsDialog.dialogInitialization();
        loadingColumn.visible = true;
    }

    Connections {
        target: finTsDialog
        onDialogInitializationCompleted: {
            finTsDialog.accountTransactions(transactionsPage.accountId);
        }
        onDialogInitializationFailed: {
            loadingColumn.visible = false;
            errorColumn.retryPossible = true;
            errorColumn.visible = true;
            errorInfoLabel.text = qsTr("Unable to connect to your bank. Please ensure that your internet connection works properly and try again.");
        }
        onAccountTransactionsCompleted: {
            finTsDialog.closeDialog();
            textContent.text = accountTransactions[0];
            loadingColumn.visible = false;
            transactionsColumn.visible = true;
        }
        onAccountTransactionsFailed: {
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
            height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1 : 0
            visible: false

            InfoLabel {
                id: loadingLabel
                text: qsTr("Retrieving transactions...")
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
                source: "../../images/zaster.svg"
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
                id: searchHeader
                title: qsTr("Transactions")
            }

            Text {
                id: textContent
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.primaryColor
                linkColor: Theme.highlightColor
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
            }

            VerticalScrollDecorator {}

        }

    }

}

