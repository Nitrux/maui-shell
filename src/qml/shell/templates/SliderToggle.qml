import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractBarToggle
{
    id: control
    property alias slider : _slider

    bar: SliderTemplate
    {
        id: _slider
        Layout.fillWidth: true
    }
}
