import QtQuick 2.6
import Grefsen 1.0

Image {
    fillMode: Image.PreserveAspectCrop
    // download from https://commons.wikimedia.org/wiki/File:Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg
    source: Env.grefsenconfig + "Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg"

    // TODO set the icon theme

    LeftSlidePanel {
        id: leftPanel

        LauncherMenuIcon { }

        LauncherIcon {
            path: "/usr/bin/konsole"
            icon: "utilities-terminal"
        }

        LauncherIcon {
            path: "/usr/bin/qtcreator"
            icon: "QtProject-qtcreator"
        }

        LauncherIcon {
            // the Qt WebKit example; replace with your favorite Wayland-compatible browser
            path: "fancybrowser"
            icon: "internet-web-browser"
        }

        LauncherIcon {
            path: "quasselclient"
            icon: "quassel"
        }

        /*
        // depends on https://git.merproject.org/mer-core/libconnman-qt
        PopoverTrayIcon {
            popover: ConnmanPopover { }
            icon: "preferences-system-network"
        }
        */

        PanelClock { }

        QuitButton { }

        Shortcut {
            sequence: "Meta+A" // maybe not the best one... or maybe we don't need it at all
            onActivated: leftPanel.toggle()
        }
    }

    Shortcut {
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }
}
