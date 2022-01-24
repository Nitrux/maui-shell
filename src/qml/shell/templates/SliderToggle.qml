import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

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
