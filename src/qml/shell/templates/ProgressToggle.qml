import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractBarToggle
{
    id: control
    property alias progressbar : _bar

    bar: ProgressBarTemplate
    {
        id: _bar
        Layout.fillWidth: true
    }
}
