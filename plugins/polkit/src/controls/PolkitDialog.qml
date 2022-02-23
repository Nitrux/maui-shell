import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import org.cask.polkit 1.0 as Polkit

import QtQuick.Templates 2.15 as T

Maui.Dialog
{
    id: control
    property Polkit.Dialog dialog

    page.margins: Maui.Style.space.big
    spacing: Maui.Style.space.medium
    page.headerColorSet: control.Kirigami.Theme.colorSet
    closeButtonVisible: false
    maxWidth: 400
    hint: 1

    entryField: true

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
