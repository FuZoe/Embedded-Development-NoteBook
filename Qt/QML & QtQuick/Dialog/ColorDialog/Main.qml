/*一个用于选取颜色的对话框示例*/
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 6.6

ApplicationWindow {
    id: window
    visible: true
    width: 400
    height: 300
    title: "Color Dialog Test"

    Button {
        text: "选择颜色"
        anchors.centerIn: parent
        onClicked: colorDialog.open()
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        onAccepted: {
            console.log("You chose: " + colorDialog.selectedColor)
            Qt.quit()
        }
        onRejected: {
            console.log("Canceled")
            Qt.quit()
        }
    }
}
