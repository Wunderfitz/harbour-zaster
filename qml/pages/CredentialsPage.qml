/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of ZasterBanker.

    ZasterBanker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ZasterBanker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ZasterBanker. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: credentialsPage
    allowedOrientations: Orientation.All

    SilicaFlickable {

        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: credentialsColumn.height

        Column {
            id: credentialsColumn
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: searchHeader
                title: qsTr("Enter your Credentials")
            }

            SectionHeader {
                text: qsTr("Your Bank")
            }

            DetailItem {
                label: qsTr("Name")
                value: finTsDialog.getBankName()
            }

            DetailItem {
                label: qsTr("Bank ID")
                value: finTsDialog.getBankId()
            }

            SectionHeader {
                text: qsTr("Your Credentials")
            }

            TextField {
                id: userNameField
                width: parent.width
                placeholderText: qsTr("User name")
                labelVisible: false
            }

            PasswordField {
                id: pinField
                width: parent.width
                placeholderText: qsTr("PIN or Password")
                labelVisible: false
            }

            Button {
                id: loginButton
                text: qsTr("Login")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                enabled: ( userNameField.text !== "" && pinField.text !== "" )
                onClicked: {
                    finTsDialog.setUserData(userNameField.text, pinField.text);
                    pageStack.clear();
                    pageStack.push(Qt.resolvedUrl("OverviewPage.qml"));
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
