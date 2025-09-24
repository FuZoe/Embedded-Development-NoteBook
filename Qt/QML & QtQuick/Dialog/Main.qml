/*一个简单的对话框
 */
import QtQuick
import QtQuick.Controls

Window {
    width: 800
    height: 640
    visible: true
    Component.onCompleted: {
        dialog.open()
    }

    Dialog {
        id: dialog
        title: "Title"
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: console.log("Ok clicked")
        onRejected: console.log("Cancel clicked")
    }
}
