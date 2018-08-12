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


CoverBackground {

    CoverActionList {
        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: {
                console.log("Refresh triggered");
            }
        }
    }

    Image {
        source: "../../images/background.png"
        anchors {
            verticalCenter: parent.verticalCenter

            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium

            right: parent.right
            rightMargin: Theme.paddingMedium
        }

        fillMode: Image.PreserveAspectFit
        opacity: 0.2
    }

    Label {
        id: statusText
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Theme.fontSizeExtraLarge
        text: "Zaster"
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }

}


