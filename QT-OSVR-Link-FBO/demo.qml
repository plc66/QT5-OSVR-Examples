/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
//import QtQuick.Particles 2.0
import QtMultimedia 5.0

Rectangle {
    id: root

    color: "black"
    /*
    Image {
        source: "file:/home/pilou/OSVR_Projects/Screenshot from 2016-02-19 19-17-01.png"
    }
    */
    Text {
        anchors.centerIn: parent
        text: "Dimensions " + parent.width + "x" + parent.height
        font.pointSize: 40
        color: "white"

        SequentialAnimation on rotation {
            //PauseAnimation { duration: 2500 } // Warning , the openGL render renders only if there is something to render in the qt application - > BLOCKING
            NumberAnimation { from: 0; to: 360; duration: 5000; easing.type: Easing.InOutCubic }
            loops: Animation.Infinite
        }
    }

    Video {
        id: video
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        anchors.fill: parent
        //source: "rtsp://10.90.90.95:554/videostream.asf?usr=admin&pwd=admin"
        //source: "rtsp://admin:admin@10.90.90.95:554/"
        source: "file:///home/pilou/OSVR_Projects/test2.avi"
        autoPlay: true
    }


    MouseArea {
        id: mouse
        anchors.fill: parent
    }
}
