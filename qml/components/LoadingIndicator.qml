/*
    Copyright (C) 2018-19 Sebastian J. Wolf

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

import QtQuick 2.0
import Sailfish.Silica 1.0

Item {

    id: loadingIndicator

    property bool withOverlay: true
    property string informationText: qsTr("Loading...")

    width: parent.width
    height: parent.height
    Rectangle {
        id: loadingOverlay
        color: "black"
        opacity: 0.7
        width: parent.width
        height: parent.height
        visible: loadingIndicator.withOverlay
    }

    Column {
        width: parent.width
        height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        InfoLabel {
            id: loadingLabel
            text: loadingIndicator.informationText
        }

        BusyIndicator {
            id: loadingBusyIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: loadingIndicator.visible
            size: BusyIndicatorSize.Large
        }
    }

}
