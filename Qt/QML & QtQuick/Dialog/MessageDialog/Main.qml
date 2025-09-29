/*一个简单的消息对话框示例
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Window {
    width: 800
    height: 640
    visible: true
    color:"white"


    Component.onCompleted: {
        messageDialog.open()
    }

    MessageDialog {
        id: messageDialog
        title: "消息提示"
        text: "这是一个消息对话框示例"
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: console.log("Ok clicked")
        onRejected: console.log("Cancel clicked")

    }
}
