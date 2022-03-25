#!/usr/bin/env bash

MAUIKIT_FILEBROWSING_VERSION=$([ -z $BRANCH_MAUIKIT_FILEBROWSING ] && echo "master" || echo $BRANCH_MAUIKIT_FILEBROWSING)

echo "Cloning Mauikit Filebrowsing from branch $MAUIKIT_FILEBROWSING_VERSION"

git clone https://invent.kde.org/maui/mauikit-filebrowsing.git --depth=1 -b $MAUIKIT_FILEBROWSING_VERSION

cd mauikit-filebrowsing
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    .

make -j$(nproc)
DESTDIR=/ make install
make install