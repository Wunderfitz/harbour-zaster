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
    id: overviewPage

    allowedOrientations: Orientation.All
    property bool initializationCompleted: false
    property bool userParameterDataRequest: false
    property variant allAccounts;
    property variant allowedTwoStepMethods;

    Component.onCompleted: {
        allAccounts = finTsAccounts.getAllAccounts();
        if (finTsDialog.isPinSet()) {
            // Initial retrieval of account information
            finTsDialog.synchronization();
            loadingColumn.visible = true;
        } else {
            loadingColumn.canAbort = true;
            overviewFlickable.visible = false;
            enterPinFlickable.visible = true;
        }
    }

    function enterPin() {
        finTsDialog.setPin(enterPinField.text);
        overviewPage.initializationCompleted = true;
        enterPinFlickable.visible = false;
        loadingColumn.visible = true;
        finTsBalances.retrieveBalances();        
    }

    function abortLoading() {
        finTsBalances.abort();
        loadingColumn.visible = false;
        enterPinFlickable.visible = true;
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
        onErrorRetrievingBalances: {
            if (pageStack.currentPage === overviewPage) {
                errorMessageRepeater.model = [{ code: "Internal", text: errorMessage }] ;
                errorFlickable.visible = true;
                enterPinFlickable.visible = false;
                overviewFlickable.visible = false;
                loadingColumn.visible = false;
            } else {
                console.log("[OverviewPage] Not handling error as not current page...")
            }
        }
    }

    Connections {
        target: finTsDialog
        onDialogEndCompleted: {
            if (!overviewPage.initializationCompleted) {
                loadingColumn.canAbort = true;
                if (finTsDialog.containsAccounts() || overviewPage.userParameterDataRequest) {
                    overviewPage.userParameterDataRequest = false;
                    overviewPage.initializationCompleted = true;
                    finTsBalances.retrieveBalances();
                } else {
                    overviewPage.userParameterDataRequest = true;
                    if (finTsDialog.requiresTwoFactorSelection()) {
                        overviewPage.allowedTwoStepMethods = finTsDialog.getAllowedTwoStepMethods();
                        loadingColumn.visible = false;
                        twoFactorMethodFlickable.visible = true;
                        console.log("[OverviewPage] Number of allowed two-step methods: " + overviewPage.allowedTwoStepMethods.length);
                        console.log("[OverviewPage] No accounts received, trying additional dialog initialization");
                    } else {
                        finTsDialog.dialogInitialization();
                    }
                }
            }
        }
        onErrorOccurred: {
            if (pageStack.currentPage === overviewPage) {
                errorMessageRepeater.model = finTsDialog.getErrorMessages();
                errorFlickable.visible = true;
                enterPinFlickable.visible = false;
                overviewFlickable.visible = false;
                loadingColumn.visible = false;
            } else {
                console.log("[OverviewPage] Not handling error as not current page...")
            }
        }
        onSynchronizationCompleted: {
            finTsDialog.closeDialog();
        }

        onDialogInitializationCompleted: {
            if (overviewPage.userParameterDataRequest) {
                finTsDialog.closeDialog();
            }
        }
    }

    Column {
        id: loadingColumn
        width: parent.width
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        property bool canAbort: false

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        Image {
            id: zasterLoadingImage            
            visible: overviewPage.isPortrait
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

        Button {
            id: loadingAbortButton
            visible: loadingColumn.canAbort
            text: qsTr("Abort")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                abortLoading();
            }
        }
    }

    SilicaFlickable {
        id: twoFactorMethodFlickable
        anchors.fill: parent
        topMargin: Theme.horizontalPageMargin
        contentHeight: twoFactorMethodColumn.height

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        Column {
            id: twoFactorMethodColumn
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            spacing: Theme.paddingMedium
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: zasterTwoFactorImage
                source: "../../images/zaster.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                id: twoFactorInfoLabel
                font.pixelSize: Theme.fontSizeLarge
                text: qsTr("Choose a two-factor method")
            }

            Text {
                id: twoFactorMessageText
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.primaryColor
                wrapMode: Text.Wrap
                text: qsTr("Remark: Zaster Banker currently only supports mobile/SMS TAN methods.")
            }

            ComboBox {
                id: twoFactorComboBox
                label: qsTr("Two-Factor Method")
                menu: ContextMenu {
                    Repeater {
                        model: allowedTwoStepMethods
                        delegate: MenuItem {
                            text: modelData.description
                        }
                    }
                    onActivated: {
                        console.log("Two-factor method " + allowedTwoStepMethods[index].id + " was selected");
                    }
                }
            }

            Button {
                id: twoFactorOkButton
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    finTsDialog.setTwoFactorMethod(allowedTwoStepMethods[twoFactorComboBox.currentIndex].id);
                    twoFactorMethodFlickable.visible = false;
                    loadingColumn.visible = true;
                    finTsDialog.dialogInitialization();
                }
            }
        }

    }

    SilicaFlickable {
        id: errorFlickable
        anchors.fill: parent
        topMargin: Theme.horizontalPageMargin
        contentHeight: errorColumn.height

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        Column {
            id: errorColumn
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            spacing: Theme.paddingMedium
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

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
                text: qsTr("Unable to retrieve account information. Please check the following error messages.")
            }

            Repeater {
                id: errorMessageRepeater
                width: parent.width
                delegate: Text {
                    id: errorMessageText
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    text: qsTr("%1 (Error Code: %2)").arg(modelData.text).arg(modelData.code)
                }
            }

            Button {
                id: errorOkButton
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    errorFlickable.visible = false;
                    enterPinFlickable.visible = true;
                }
            }
        }

    }

    SilicaFlickable {
        id: enterPinFlickable
        anchors.fill: parent
        topMargin: Theme.horizontalPageMargin
        contentHeight: enterPinColumn.height

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        RemorsePopup {
            id: removeAccountRemorsePopup
        }

        onVisibleChanged: {
            if (visible) {
                allAccounts = finTsAccounts.getAllAccounts();
            }
        }

        Column {
            id: enterPinColumn

            width: parent.width
            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: zasterImage
                source: "../../images/zaster.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            Text {
                id: bankInfoLabel
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeLarge
                horizontalAlignment: Text.AlignHCenter
                color: Theme.primaryColor
                wrapMode: Text.Wrap
                text: finTsDialog.getBankName()
            }

            Text {
                id: pinInfoLabel
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeMedium
                horizontalAlignment: Text.AlignHCenter
                color: Theme.secondaryColor
                wrapMode: Text.Wrap
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
                enabled: enterPinColumn.visible
                placeholderText: qsTr("Your PIN or Password")
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

            Separator {
                id: accountsSeparator
                visible: allAccounts.length > 1

                width: parent.width
                color: Theme.primaryColor
                horizontalAlignment: Qt.AlignHCenter
            }

            ComboBox {
                id: accountsComboBox
                visible: allAccounts.length > 1
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
                        enterPinFlickable.visible = false;
                        finTsAccounts.switchAccount(allAccounts[index].uuid);
                        bankInfoLabel.text = finTsDialog.getBankName();
                        enterPinFlickable.visible = true;
                    }
                }
            }

            Button {
                id: removeAccountButton
                text: qsTr("Remove this Account")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    removeAccountRemorsePopup.execute(qsTr("Removing account"), function() {
                        finTsAccounts.removeCurrentAccount();
                        pageStack.clear();
                        pageStack.push(finTsDialog.isInitialized() ? overviewPage : institutesSearchPage);
                    } );
                }
            }
        }

        VerticalScrollDecorator {}
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
                text: qsTr("About Zaster Banker")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
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

            Image {
                id: overviewImage
                source: "../../images/zaster.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                visible: overviewPage.isPortrait

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            Text {
                id: bankNameText
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.primaryColor
                wrapMode: Text.Wrap
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

                height: overviewPage.height - accountsHeader.height - bankNameText.height - bankCodeText.height - ( 3 * Theme.paddingMedium ) - ( overviewPage.isPortrait ? overviewImage.height + Theme.paddingMedium : 0 )
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
                        pageStack.push(Qt.resolvedUrl("AccountTransactionsPage.qml"), {"accountId": modelData.accountId, "iban": modelData.iban})
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
