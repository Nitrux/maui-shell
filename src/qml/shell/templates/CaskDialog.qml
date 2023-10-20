import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui

import org.maui.cask 1.0 as Cask
import QtQuick.Templates 2.15 as T

Maui.PopupPage
{
    id: control
    spacing: Maui.Style.space.medium
    page.headerColorSet: control.Maui.Theme.colorSet
    closeButtonVisible: false
    maxWidth: 400
    hint: 1
}
