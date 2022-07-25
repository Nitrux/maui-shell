#!/usr/bin/env bash

CASK_SERVER_VERSION=$([ -z $BRANCH_CASK_SERVER ] && echo "main" || echo $BRANCH_CASK_SERVER)

echo "Cloning Cask Server from branch $CASK_SERVER_VERSION"

git clone https://github.com/Nitrux/cask-server.git --depth=1 -b $CASK_SERVER_VERSION

cd cask-server
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DBUNDLE_MAUI_STYLE=ON \
    .

make -j$(nproc)
DESTDIR=/ make install
make install
