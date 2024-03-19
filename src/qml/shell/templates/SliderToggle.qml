import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractBarToggle
{
    id: control
    readonly property alias slider : _slider

    bar: SliderTemplate
    {
        id: _slider
        Layout.fillWidth: true
    }
}
