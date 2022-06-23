#!/usr/bin/env bash

MAUIMAN_VERSION=$([ -z $BRANCH_MAUIMAN ] && echo "master" || echo $BRANCH_MAUIMAN)

echo "Cloning Mauiman from branch $MAUIMAN_VERSION"

git clone https://invent.kde.org/maui/mauiman.git --depth=1 -b $MAUIMAN_VERSION

cd mauiman
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DBUNDLE_MAUI_STYLE=ON \
    .

make -j$(nproc)
DESTDIR=/ make install
make install