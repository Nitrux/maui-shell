import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

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
