import QtQuick 2.13
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import QtGraphicalEffects 1.0
import ".."

Item
{
    id: control
    clip: true

    Component
    {
     id: _chrome


     Chrome
     {
         id: _chromeDelegate
//         shellSurface: modelData
         SplitView.fillHeight: true
         SplitView.fillWidth: true
         SplitView.minimumHeight: 500
         SplitView.minimumWidth: 500

//         moveItem: Item
//         {
//             property bool moving: false
//             parent: surfaceArea
//             x: output.position.x
//             y: output.position.y
//             height: _chromeDelegate.shellSurface.surface.height
//             width: _chromeDelegate.shellSurface.surface.width
//         }
     }
    }

    SplitView
    {
        id: _splitView
      anchors.fill: parent
      orientation: Qt.Horizontal
      Rectangle
      {
          color: "orange"
          SplitView.fillHeight: true
          SplitView.fillWidth: true
          SplitView.minimumHeight: 100
          SplitView.minimumWidth: 100
      }


      Rectangle
      {
          color: "blue"
          SplitView.fillHeight: true
          SplitView.fillWidth: true
          SplitView.minimumHeight: 100
          SplitView.minimumWidth: 100
      }
    }

    function insert(item)
    {
_splitView.insertItem(1,_chrome.createObject(_splitView, { shellSurface: item}))
    }

}
