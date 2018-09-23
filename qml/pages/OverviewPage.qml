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

    Component.onCompleted: {
        if (finTsDialog.isPinSet()) {
            finTsDialog.dialogInitialization();
        } else {
            overviewFlickable.visible = false;
        }
    }

    Connections {
        target: finTsDialog
        onDialogInitializationCompleted: {
            bankNameText.text = finTsDialog.getBankName();
            bankCodeText.text = qsTr("Bank ID: %1").arg(finTsDialog.getBankId());
            finTsDialog.storeParameterData();
            finTsDialog.accountBalance();
        }
        onDialogEndCompleted: {

        }
        onAccountBalanceCompleted: {
            console.log("Retrieved account balances: " + accountBalances.length);
            accountIdText.text = qsTr("Account ID: %1").arg(accountBalances[0].accountId);
            accountValueText.text = qsTr("%1 %2").arg(accountBalances[0].value).arg(accountBalances[0].currency);
            finTsDialog.closeDialog();
        }
    }

    Column {

        id: enterPinColumn

        width: parent.width
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: !overviewFlickable.visible

        Image {
            id: zasterImage
            source: "../../images/zaster.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: pinInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Please enter your PIN or Password")
        }

        PasswordField {
            id: enterPinField
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            font.pixelSize: Theme.fontSizeLarge
            width: parent.width
            horizontalAlignment: TextInput.AlignHCenter
        }

        Button {
            id: pinOkButton
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                finTsDialog.setPin(enterPinField.text);
                overviewFlickable.visible = true;
                finTsDialog.dialogInitialization();
            }
        }
    }


    SilicaFlickable {
        id: overviewFlickable
        anchors.fill: parent
        contentHeight: overviewColumn.height

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

            Text {
                id: accountIdText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                id: accountValueText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraLarge
                color: Theme.highlightColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

    }

}
