import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.cask.polkit as Polkit

Dialog
{
    id: control
    property Polkit.Dialog dialog

    title: i18n("Authenticate")
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem: ColumnLayout
    {
        spacing: 8

        Label
        {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: control.dialog ? control.dialog.message : ""
        }

        TextField
        {
            id: passwordField
            Layout.fillWidth: true
            placeholderText: i18n("Password")
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
            focus: true
            onAccepted: control.accept()
        }
    }

    onAccepted: submit(passwordField.text)
    onRejected: submit("")

    function submit(password)
    {
        if (control.dialog)
            control.dialog.setConfirmationResult(password)

        control.close()
    }
}
