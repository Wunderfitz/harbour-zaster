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
    id: overviewPage

    allowedOrientations: Orientation.All
    property bool inErrorStatus: false
    property bool initializationCompleted: false

    Component.onCompleted: {
        if (finTsDialog.isPinSet()) {
            // Initial retrieval of account information
            finTsDialog.synchronization();
            loadingColumn.visible = true;
        } else {
            overviewFlickable.visible = false;
            enterPinColumn.visible = true;
        }
    }

    function enterPin() {
        finTsDialog.setPin(enterPinField.text);
        overviewPage.initializationCompleted = true;
        finTsBalances.retrieveBalances();
        enterPinColumn.visible = false;
        loadingColumn.visible = true;
    }

    Connections {
        target: finTsBalances
        onBalancesRetrieved: {
            bankNameText.text = finTsDialog.getBankName();
            bankCodeText.text = qsTr("Bank ID: %1").arg(finTsDialog.getBankId());
            console.log("Retrieved account balances: " + accountBalances.length);
            accountsListView.model = accountBalances;
            loadingColumn.visible = false;
            overviewFlickable.visible = true;
        }
    }

    Connections {
        target: finTsDialog
        onDialogEndCompleted: {
            if (!overviewPage.initializationCompleted) {
                overviewPage.initializationCompleted = true;
                finTsBalances.retrieveBalances();
            }
        }
        onErrorOccurred: {
            overviewPage.inErrorStatus = true;
            errorColumn.visible = true;
            enterPinColumn.visible = false;
            overviewFlickable.visible = false;
            loadingColumn.visible = false;
        }
        onSynchronizationCompleted: {
            finTsDialog.closeDialog();
        }
    }

    Column {
        id: loadingColumn
        width: parent.width
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

        id: errorColumn
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
            text: qsTr("Unable to retrieve account information. Please ensure that you entered proper credentials and try again.")
        }

        Button {
            id: errorOkButton
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                overviewPage.inErrorStatus = false;
                errorColumn.visible = false;
                enterPinColumn.visible = true;
            }
        }
    }

    Column {

        id: enterPinColumn

        width: parent.width
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        Image {
            id: zasterImage
            source: "../../images/zaster.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: pinInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Authentication required")
        }

        PasswordField {
            id: enterPinField
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            font.pixelSize: Theme.fontSizeLarge
            width: parent.width
            horizontalAlignment: TextInput.AlignHCenter
            labelVisible: false
            placeholderText: qsTr("Your PIN or Password")
            focus: enterPinColumn.visible
            Keys.onEnterPressed: {
                if (pinOkButton.enabled) {
                    enterPin();
                }
            }
            Keys.onReturnPressed: {
                if (pinOkButton.enabled) {
                    enterPin();
                }
            }
        }

        Button {
            id: pinOkButton
            text: qsTr("OK")
            enabled: enterPinField.text !== ""
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                enterPin();
            }
        }
    }


    SilicaFlickable {
        id: overviewFlickable
        anchors.fill: parent
        contentHeight: overviewColumn.height

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        PullDownMenu {
            MenuItem {
                text: qsTr("About Zaster")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        Column {
            id: overviewColumn

            width: overviewPage.width
            spacing: Theme.paddingMedium
            PageHeader {
                id: accountsHeader
                title: qsTr("Account Balance")
            }

            Text {
                id: bankNameText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                id: bankCodeText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            SilicaListView {

                id: accountsListView

                height: overviewPage.height - accountsHeader.height - bankNameText.height - bankCodeText.height - ( 3 * Theme.paddingMedium )
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right

                clip: true

                delegate: ListItem {
                    contentHeight: resultItem.height + Theme.paddingMedium
                    contentWidth: parent.width

                    enabled: finTsDialog.canRetrieveTransactions(modelData.accountId) || finTsDialog.canRetrievePortfolioInfo(modelData.accountId)

                    onClicked: {
                        console.log("Selected: " + modelData.accountId);
                        pageStack.push(Qt.resolvedUrl("AccountTransactionsPage.qml"), {"accountId": modelData.accountId})
                    }

                    Item {
                        id: resultItem
                        width: parent.width
                        height: resultRow.height + accountSeparator.height + Theme.paddingMedium

                        Row {
                            id: resultRow
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            spacing: Theme.paddingMedium
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            Column {
                                width: parent.width / 2 - Theme.paddingSmall
                                Text {
                                    id: accountIdText
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    text: modelData.accountId
                                }
                                Text {
                                    id: accountDescriptionText
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    text: modelData.accountDescription
                                }
                            }
                            Text {
                                id: accountValueText
                                width: parent.width / 2 - Theme.paddingSmall
                                height: parent.height
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: Theme.fontSizeMedium
                                color: Theme.highlightColor
                                text: (modelData.creditDebit === "D" ? "- " : "") + Number(modelData.value).toLocaleString(Qt.locale(), "f", 2) + " " + modelData.currency
                            }
                        }

                        Separator {
                            id: accountSeparator
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
