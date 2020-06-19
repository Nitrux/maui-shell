import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import MeeGo.Connman 0.2

Cask.PanelItem
{
    icon.name: "network-wireless"

    NetworkManager
    {
        id: manager
        function checkTechnologies() {
            var tl = manager.technologiesList()
            if (tl.length > 0)
                techList.model = tl
            console.log("state " + state + " available? " + available + " enabled svcs " + servicesEnabled + " techs " + technologiesEnabled)
            console.log("   list " + tl)
        }

        onAvailabilityChanged: checkTechnologies()
        onStateChanged: checkTechnologies()
        onOfflineModeChanged: checkTechnologies()
        onTechnologiesChanged: checkTechnologies()
        onTechnologiesEnabledChanged: checkTechnologies()
    }

    card: Cask.PanelCard
    {
      width: parent.width
      implicitHeight: 500
      padding: 0
      title: qsTr("Network")
      headBar.visible: true
      headBar.leftContent: [
          CheckBox
          {
              icon.name: "nm-device-wireless"

          },
          ToolButton
          {
              icon.name: "airplane-mode"
          }

      ]
      headBar.rightContent: ToolButton
      {
          icon.name: "configure"
      }


      SwipeView
      {
          id: swipeView

          currentIndex: 0
          anchors.fill: parent

          ListView
          {
              id: techList
              spacing: 2
              delegate: Rectangle {
                  width: parent.width
                  height: 40
                  color: "#222"
                  property NetworkTechnology tech: manager.getTechnology(modelData)
                  Text {
                      text: tech.name
                      color: "white"
                      anchors.verticalCenter: parent.verticalCenter
                      x: 6
                  }
                  MouseArea {
                      anchors.fill: parent
                      enabled: modelData === "wifi" && techEnableSwitch.checked
                      onClicked: swipeView.currentIndex = 1
                  }
                  Switch {
                      id: techEnableSwitch
                      checked: tech.powered
                      anchors {
                          right: parent.right
                          margins: 6
                          verticalCenter: parent.verticalCenter
                      }
                      onReleased: {
                          console.log("setting " + modelData + " " + !checked)
                          tech.powered = !checked
                      }
                  }
              }
          }
          ListView {
              id: wifiList
              spacing: 2
              model: TechnologyModel {
                  id: techModel
                  name: "wifi"
                  onAvailabilityChanged: {
                      console.log(name + " powered? " + powered + " available? " + available)
                  }
              }
              delegate: Rectangle {
                  width: parent.width
                  height: 40
                  color: "#222"
                  Text {
                      text: networkService.name
                      color: "white"
                      anchors.verticalCenter: parent.verticalCenter
                      x: 6
                  }
                  Text {
                      anchors {
                          right: parent.right
                          margins: 6
                          verticalCenter: parent.verticalCenter
                      }
                      color: "white"
                      font.pixelSize: 30
                      text: networkService.connected ? "✔" : ""
                  }
                  MouseArea {
                      anchors.fill: parent
                      enabled: !networkService.connected
                      onClicked: {
                          console.log("trying to connect to " + networkService.name)
                          networkService.requestConnect()
                      }
                  }
              }
          }
          Item {
              id: wifiConfigPage
              Rectangle {
                  anchors.fill: parent
                  anchors.margins: 6
                  anchors.bottomMargin: indicator.height
                  radius: 8
                  color: "#444"
              }
          }
      }

      PageIndicator {
          id: indicator

          count: swipeView.count
          currentIndex: swipeView.currentIndex

          anchors.bottom: swipeView.bottom
          anchors.horizontalCenter: parent.horizontalCenter
      }
    }
}
