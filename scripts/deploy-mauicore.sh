#!/usr/bin/env bash

MAUICORE_VERSION=$([ -z $BRANCH_MAUICORE ] && echo "main" || echo $BRANCH_MAUICORE)

echo "Cloning Mauicore from branch $MAUICORE_VERSION"

git clone https://github.com/Nitrux/maui-core.git --depth=1 -b $MAUICORE_VERSION

cd maui-core
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DBUNDLE_MAUI_STYLE=ON \
    .

make -j$(nproc)
DESTDIR=/ make install
make install