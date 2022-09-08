import QtQuick 2.15
import QtQuick.Controls 2.15

import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
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
