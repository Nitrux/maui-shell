import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.cask.polkit as Polkit

Maui.InfoDialog
{
    id: control
    property Polkit.Dialog dialog

    template.iconSource: "unlock"
    title: i18n("Authenticate")
    message: control.dialog.message

    onAccepted:
    {
        if(control.textEntry.text)
        {
            control.dialog.setConfirmationResult(control.textEntry.text)
        }
    }

    onRejected:
    {
        control.dialog.setConfirmationResult("")
        control.close()
    }
}
