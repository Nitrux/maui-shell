#! /bin/bash

set -x

### Update sources

wget -qO /etc/apt/sources.list.d/neon-user-repo.list https://raw.githubusercontent.com/Nitrux/iso-tool/development/configs/files/sources.list.neon.user

wget -qO /etc/apt/sources.list.d/nitrux-main-compat-repo.list https://raw.githubusercontent.com/Nitrux/iso-tool/development/configs/files/sources.list.nitrux

wget -qO /etc/apt/sources.list.d/nitrux-testing-repo.list https://raw.githubusercontent.com/Nitrux/iso-tool/development/configs/files/sources.list.nitrux.testing

DEBIAN_FRONTEND=noninteractive apt-key adv --keyserver keyserver.ubuntu.com --recv-keys \
	55751E5D > /dev/null

curl -L https://packagecloud.io/nitrux/repo/gpgkey | apt-key add -;
curl -L https://packagecloud.io/nitrux/compat/gpgkey | apt-key add -;
curl -L https://packagecloud.io/nitrux/testing/gpgkey | apt-key add -;

DEBIAN_FRONTEND=noninteractive apt -qq update

### Install Package Build Dependencies #2

DEBIAN_FRONTEND=noninteractive apt -qq -yy install --no-install-recommends \
	cask-server-git \
	mauikit-git \
	mauikit-filebrowsing-git

### Download Source

git clone --depth 1 --branch $MAUISHELL_BRANCH https://github.com/Nitrux/maui-shell.git

rm -rf maui-shell/{screenshots,wallpapers,scripts,LICENSE,README.md,maui-shell-appimage.yml}

### Compile Source

mkdir -p build && cd build

cmake \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DENABLE_BSYMBOLICFUNCTIONS=OFF \
	-DQUICK_COMPILER=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_SYSCONFDIR=/etc \
	-DCMAKE_INSTALL_LOCALSTATEDIR=/var \
	-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON \
	-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON \
	-DCMAKE_INSTALL_RUNSTATEDIR=/run "-GUnix Makefiles" \
	-DCMAKE_VERBOSE_MAKEFILE=ON \
	-DCMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake/MauiKit;/usr/lib/x86_64-linux-gnu/cmake/MauiKitFileBrowsing" ../maui-shell/

make -j$(nproc)

### Run checkinstall and Build Debian Package

>> description-pak printf "%s\n" \
	'Maui Shell is a convergent shell for desktop, tablets and phones.' \
	'' \
	'Cask is the shell container and elements templates, such as panels, popups, cards etc.' \
	'' \
	'Zpace is the composer, which is the layout and places the windows' \
	'surfaces into the Cask container.' \
	'' \
	''

checkinstall -D -y \
	--install=no \
	--fstrans=yes \
	--pkgname=maui-shell-git \
	--pkgversion=$PACKAGE_VERSION \
	--pkgarch=amd64 \
	--pkgrelease="1" \
	--pkglicense=LGPL-3 \
	--pkggroup=utils \
	--pkgsource=maui-shell \
	--pakdir=. \
	--maintainer=uri_herrera@nxos.org \
	--provides=maui-shell \
	--requires="agenda-git \(\>= 2.2.0+git\),bluedevil,cask-server-git,kded5,kinit,libc6,libfontconfig1,libkf5activities5,libkf5activitiesstats1,libkf5archive5,libkf5configcore5,libkf5coreaddons5,libkf5crash5,libkf5declarative5,libkf5doctools5,libkf5i18n5,libkf5idletime5,libkf5itemmodels5,libkf5kiocore5,libkf5notifications5,libkf5notifyconfig5,libkf5package5,libkf5people5,libkf5prison5,libkf5runner5,libkf5runner5,libkf5service5,libkf5su5,libkf5syntaxhighlighting5,libkf5texteditor5,libkf5unitconversion5,libkf5wallet5,libkf5waylandclient5,libkf5waylandserver5,libphonon4qt5-4,libpolkit-agent-1-0,libpolkit-agent-1-0,libpolkit-qt5-1-1,libqt5core5a,libqt5gui5,libqt5qml5,libqt5svg5,libqt5waylandcompositor5,libqt5widgets5,libstdc++6,libwayland-bin,libwayland-client0,libwayland-cursor0,libwayland-egl1,libwayland-server0,mauikit-filebrowsing-git \(\>= 2.2.0+git\),mauikit-git \(\>= 2.2.0+git\),plasma-nm,plasma-pa,qml-module-qt-labs-calendar,qml-module-qt-labs-platform,qml-module-qtwayland-compositor,qtwayland5" \
	--nodoc \
	--strip=no \
	--stripso=yes \
	--reset-uids=yes \
	--deldesc=yes
