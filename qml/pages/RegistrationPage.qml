/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster Banker.

    Zaster Banker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Zaster Banker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster Banker. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.2
import Sailfish.Silica 1.0
import "../components"

Page {
    id: registrationPage
    allowedOrientations: Orientation.All

    property string productName: "Zaster Banker"
    property string productDescription: "a banking app for Sailfish OS"
    property string technicalName: "zaster"

    function getTimeRemaining(count) {
        if (count >= 172800) {
            return qsTr("the next %1 days").arg((count / 86400).toLocaleString(Qt.locale(), 'f', 0));
        } else if (count >= 86400) {
            return qsTr("the next day");
        } else if (count >= 7200 ) {
            return qsTr("the next %1 hours").arg((count / 3600).toLocaleString(Qt.locale(), 'f', 0));
        } if (count >= 3600 ) {
            return qsTr("the next hour");
        } else if (count >= 120 ) {
            return qsTr("the next %1 minutes").arg((count / 60).toLocaleString(Qt.locale(), 'f', 0));
        } else {
            return qsTr("the next minute");
        }
    }

    property bool registrationLoading : false;

    Component.onCompleted: {
        if (wagnis.isRegistered()) {
            contributionFlickable.visible = true;
            contributionFlickable.opacity = 1;
        } else {
            registrationFlickable.visible = true;
            registrationFlickable.opacity = 1;
        }
    }

    Connections {
        target: wagnis
        onRegistrationError: {
            registrationPage.registrationLoading = false;
            registrationErrorFlickable.visible = true;
            registrationErrorFlickable.opacity = 1;
        }
        onRegistrationInvalid: {
            registrationPage.registrationLoading = false;
            registrationInvalidFlickable = true;
            registrationInvalidFlickable.opacity = 1;
        }
        onRegistrationValid: {
            registrationPage.registrationLoading = false;
            if (wagnis.hasFeature("contribution")) {
                pageStack.clear();
                finTsDialog.isInitialized() ? pageStack.push(overviewPage) : pageStack.push(institutesSearchPage);
            } else {
                contributionFlickable.visible = true;
                contributionFlickable.opacity = 1;
            }
        }
        onContributionValidated: {
            registrationPage.registrationLoading = false;
            contributionValidFlickable.visible = true;
            contributionValidFlickable.opacity = 1;
        }
        onContributionValidationError: {
            registrationPage.registrationLoading = false;
            contributionInvalidFlickable.visible = true;
            contributionInvalidFlickable.opacity = 1;
        }
    }


    Column {
        anchors {
            fill: parent
        }

        id: registrationLoadingColumn
        Behavior on opacity { NumberAnimation {} }
        opacity: registrationPage.registrationLoading ? 1 : 0
        visible: registrationPage.registrationLoading ? true : false

        LoadingIndicator {
            id: registrationLoadingIndicator
            visible: registrationPage.registrationLoading
            Behavior on opacity { NumberAnimation {} }
            opacity: registrationPage.registrationLoading ? 1 : 0
            height: parent.height
            width: parent.width
            withOverlay: false
        }
    }


    SilicaFlickable {
        id: registrationFlickable
        anchors.fill: parent
        contentHeight: termsOfUseColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: termsOfUseColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Welcome to %1!").arg(productName)
            }

            Image {
                id: productImage
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            SectionHeader {
                text: qsTr("Committed to Free Software")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Welcome to %1 - %2! %1 is <a href=\"https://www.gnu.org/philosophy/free-sw\">free software</a> and licensed under the conditions of the <a href=\"https://www.gnu.org/licenses/gpl.html\">General Public License in version 3 (GPLv3)</a>. By using this software, you accept the terms and conditions of this license.").arg(productName).arg(productDescription)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            SectionHeader {
                text: qsTr("Anonymous Registration")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Moreover, %1 will register this installation automatically on our server if you press 'Accept'. We respect your privacy, therefore only anonymous and a very limited set of information will be transmitted: a unique identifer for this installation and your country. We simply would like to know how many users %1 has and where they come from. If this is not OK for you, simply close the application. No information has been transmitted so far.").arg(productName)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                id: candidateRegistrationData
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.primaryColor
                textFormat: Text.StyledText
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                visible: false
            }

            Button {
                text: qsTr("Show Registration Data")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    candidateRegistrationData.text = wagnis.getCandidateRegistrationData();
                    visible = false;
                    candidateRegistrationData.visible = true;
                }
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("The data isn't shared with anyone else. Please see the <a href=\"https://werkwolf.eu/terms.html\">legal notice</a> for additional information how the data is processed. For additional details about the motivation for this, the unique ID and how it is generated please see my page <a href=\"https://werkwolf.eu/terms.html#wagnis\">about Wagnis</a>.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }

            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Have fun with %1! Sebastian J. Wolf").arg(productName)
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Accept")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationFlickable.opacity = 0;
                    registrationFlickable.visible = false;
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

        }

    }


    SilicaFlickable {
        id: registrationErrorFlickable
        anchors.fill: parent
        contentHeight: registrationErrorColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0


        Column {
            id: registrationErrorColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Registration Error")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Registration failed. Please ensure that your device is connected to the Internet and press 'Restart Registration'. In case a restart doesn't work, please contact me via <a href=\"mailto:contact@werkwolf.eu\">E-Mail</a>")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Restart Registration")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationErrorFlickable.visible = false;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: registrationInvalidFlickable
        anchors.fill: parent
        contentHeight: registrationInvalidColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: registrationInvalidColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Registration Invalid")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("The registration file on your device is corrupt. The registration process needs to be restarted. Please ensure that your device is connected to the Internet and press 'Restart Registration'. In case the new registration isn't successful, please contact me via <a href=\"mailto:contact@werkwolf.eu\">E-Mail</a>")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Restart Registration")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationInvalidFlickable.visible = false;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: contributionFlickable
        anchors.fill: parent
        contentHeight: contributionColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        property bool canSkip: false
        property string remainingTime

        onVisibleChanged: {
            if (visible) {
                contributionFlickable.canSkip = wagnis.inTestingPeriod();
                contributionFlickable.remainingTime = getTimeRemaining(wagnis.getRemainingTime());
            }
        }

        Column {
            id: contributionColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Development Contribution")
            }

            Image {
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            SectionHeader {
                text: qsTr("Please contribute to %1").arg(productName)
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("%1 is and will remain Open Source Software. However, in order to support the development of %1, we'd like to ask you for a contribution before you can run it. Such a contribution can be made by translating %1 to another language, help developing it or simply by purchasing a key.").arg(productName)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("You have the choice between 4 different contributions: S, M, L, XL. No matter which size you choose, the purchased key will unlock the complete application. It's up to you to decide how much the app is worth to you.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
                visible: false
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("If you've already contributed to %1, please <a href=\"mailto:contact@werkwolf.eu\">contact me</a> directly.").arg(productName)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ComboBox {
                id: contributionComboBox
                label: qsTr("Contribution")
                description: qsTr("Choose your contribution here")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("%1 S - 3,33 €").arg(productName)
                    }
                    MenuItem {
                        text: qsTr("%1 M - 5,55 €").arg(productName)
                     }
                     MenuItem {
                        text: qsTr("%1 L - 7,77 €").arg(productName)
                     }
                     MenuItem {
                        text: qsTr("%1 XL - 9,99 €").arg(productName)
                     }
                }
                currentIndex: 1
            }

            Button {
                text: qsTr("Purchase Contribution Key")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    var payment;
                    var purchaseLink;
                    switch (contributionComboBox.currentIndex) {
                        case 0:
                            payment = "3,33 €";
                            purchaseLink = "https://www.digistore24.com/product/249730";
                            break;
                        case 1:
                            payment = "5,55 €";
                            purchaseLink = "https://www.digistore24.com/product/249734";
                            break;
                        case 2:
                            payment = "7,77 €";
                            purchaseLink = "https://www.digistore24.com/product/249736";
                            break;
                        case 3:
                            payment = "9,99 €";
                            purchaseLink = "https://www.digistore24.com/product/249737";
                            break;
                    }
                    console.log("Payment selected: " + payment);
                    Qt.openUrlExternally(purchaseLink);
                }
            }

            SectionHeader {
                text: qsTr("Validate your contribution key")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("After you have received your key by purchasing it or for another contribution, please validate your key here to run %1.").arg(productName)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("If you have already validated your key on another device, you can reuse it for up to 3 additional devices without the need of another contribution.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Column {
                width: parent.width

                TextField {
                    id: contributionKeyTextField
                    width: parent.width
                    labelVisible: false
                    placeholderText: "abcd-efgh-1234-5678"
                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                }

                Text {
                    text: qsTr("Enter the contribution key here")
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                }
            }

            Button {
                text: qsTr("Validate Contribution Key")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.validateContribution(contributionKeyTextField.text);
                    registrationPage.registrationLoading = true;
                    contributionFlickable.opacity = 0;
                    contributionFlickable.visible = false;
                }
            }

            Text {
                visible: contributionFlickable.canSkip
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("You can skip the contribution for %1 if you want to test %2 before.").arg(contributionFlickable.remainingTime).arg(productName)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                visible: contributionFlickable.canSkip
                text: qsTr("Continue Testing")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    pageStack.clear();
                    finTsDialog.isInitialized() ? pageStack.push(overviewPage) : pageStack.push(institutesSearchPage);
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: contributionInvalidFlickable
        anchors.fill: parent
        contentHeight: contributionInvalidColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: contributionInvalidColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Contribution not validated!")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Your contribution could not be validated. Please ensure that your device is connected to the Internet and press 'Restart Validation'. In case you have a valid contribution key and it can't be validated, please contact me via <a href=\"mailto:contact@werkwolf.eu\">E-Mail</a>")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Restart Validation")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    contributionInvalidFlickable.opacity = 0;
                    contributionInvalidFlickable.visible = false;
                    contributionFlickable.visible = true;
                    contributionFlickable.opacity = 1;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: contributionValidFlickable
        anchors.fill: parent
        contentHeight: contributionValidColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: contributionValidColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/" + technicalName + ".png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Thank You!")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Your contribution was successfully validated. Thank you very much for your contribution to the development of %1!").arg(productName)
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.getApplicationRegistration();
                    registrationPage.registrationLoading = true;
                    contributionValidFlickable.opacity = 0;
                    contributionValidFlickable.visible = false;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

}
