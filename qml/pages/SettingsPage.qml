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


Page {
    id: settingsPage
    allowedOrientations: Orientation.All

    property variant allAccounts;

    Component.onCompleted: {
        allAccounts = finTsAccounts.getAllAccounts();
    }

    RemorsePopup {
        id: removeAccountRemorsePopup
    }

    SilicaFlickable {
        id: settingsContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: settingsPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Accounts")
            }

            ComboBox {
                id: accountsComboBox
                label: qsTr("Account")
                currentIndex: 0
                description: qsTr("Choose the active account here")
                menu: ContextMenu {
                    Repeater {
                        model: allAccounts
                        delegate: MenuItem {
                            text: qsTr("%1 - %2").arg(modelData.descriptorText).arg(modelData.descriptorId)
                        }
                    }
                    onActivated: {
                        console.log("Account " + allAccounts[index].uuid + " was selected");
                        finTsAccounts.switchAccount(allAccounts[index].uuid);
                        pageStack.clear();
                        pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ?  ( finTsDialog.isInitialized() ? overviewPage : institutesSearchPage ) : registrationPage);
                    }
                }
            }

            Button {
                id: removeAccountButton
                text: qsTr("Remove current Account")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    removeAccountRemorsePopup.execute(qsTr("Removing account"), function() {
                        finTsAccounts.removeCurrentAccount();
                        pageStack.clear();
                        pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ?  ( finTsDialog.isInitialized() ? overviewPage : institutesSearchPage ) : registrationPage);
                    } );
                }
            }

            Button {
                id: registerNewAccountButton
                text: qsTr("Login with other Account")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    finTsAccounts.registerNewAccount();
                    pageStack.clear();
                    pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ?  ( finTsDialog.isInitialized() ? overviewPage : institutesSearchPage ) : registrationPage);
                }
            }

            SectionHeader {
                text: qsTr("Others")
            }

            Slider {
                id: daysToRetrieveLabel
                label: qsTr("Transactions: Retrieval Period")
                minimumValue: 5
                maximumValue: 90
                stepSize: 5
                width: parent.width
                value: finTsDialog.getTransactionsSince();
                valueText: qsTr("%1 days").arg(value)
                onValueChanged: finTsDialog.setTransactionsSince(value)
            }

            Label {
                id: separatorLabel
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
