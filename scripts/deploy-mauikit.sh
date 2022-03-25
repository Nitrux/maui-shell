#!/usr/bin/env bash

MAUIKIT_VERSION=$([ -z $BRANCH_MAUIKIT ] && echo "master" || echo $BRANCH_MAUIKIT)

echo "Cloning Mauikit from branch $MAUIKIT_VERSION"

git clone https://invent.kde.org/maui/mauikit.git --depth=1 -b $MAUIKIT_VERSION

cd mauikit
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DBUNDLE_MAUI_STYLE=ON \
    .

make -j$(nproc)
DESTDIR=/ make install
make install