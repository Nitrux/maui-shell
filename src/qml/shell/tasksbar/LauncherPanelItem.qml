import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelItem
{
    icon.name: "start-here"
    iconSize: 48
    //            visible: !isMobile

    card: Cask.PanelCard
    {
        implicitWidth: parent.width
        implicitHeight: Math.min(_cask.avaliableHeight, 800)
        background: Item{}
        Launcher
        {
            id: _launcherGrid
            height: parent.height
            width: Math.min(800, parent.width)
            anchors.centerIn: parent

//           scale: parent.opacity
        }
    }
}
