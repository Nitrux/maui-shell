import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

import org.maui.cask as Cask
import "../.."

ToggleTemplate
{
    id: control
    text: i18n("Screenshot")
    icon.name:  "edit-image-face-detect"
    page: ScreenshotPage
    {

    }

}
