/*一个用Drawer实现的侧边栏菜单功能*/
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 600
    height: 480
    visible: true

    // 三个状态：0=收起, 1=半展开, 2=全展开
    property int drawerState: 0

    Drawer {
        id: drawer
        width: drawerState === 0 ? 0 : (drawerState === 1 ? 150 : 250)
        height: parent.height
        edge: Qt.LeftEdge
        modal: false
        interactive: false
        visible: drawerState > 0

        Rectangle {
            anchors.fill: parent
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "侧边栏\n状态: " + drawerState
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                visible: drawerState > 0
            }
        }
    }

    // 控制按钮
    Column {
        id:buttons
        anchors.verticalCenter: parent.verticalCenter  // 垂直居中
        anchors.horizontalCenter: parent.horizontalCenter  // 水平居中
        anchors.horizontalCenterOffset: drawerState === 1 ? 150 : 0  // 向右偏移200像素
        spacing: 10
        transform: Scale {
            xScale: drawerState === 1 ? 0.75 : 1.0   // 宽度缩放到75%
            yScale: 1.0    // 高度保持100%
        }

        Rectangle {
            width: buttons.width *0.5
            height: 40
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "切换状态"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: drawerState = (drawerState + 1) % 3
            }
        }

        Row {
            spacing: 10

            Rectangle {
                width: buttons.width  * 0.25
                height: 30
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "收起"
                    font.pixelSize: 12
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: drawerState !== 0
                    onClicked: drawerState = 0
                }
            }

            Rectangle {
                width: buttons.width * 0.25
                height: 30
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "半展开"
                    font.pixelSize: 12
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: drawerState !== 1
                    onClicked: drawerState = 1
                }
            }

            Rectangle {
                width: 60
                height: 30
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "全展开"
                    font.pixelSize: 12
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: drawerState !== 2
                    onClicked: drawerState = 2
                }
            }
        }
    }
}
