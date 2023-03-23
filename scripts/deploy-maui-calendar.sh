#!/usr/bin/env bash

MAUI_CALENDAR_VERSION=$([ -z $BRANCH_MAUI_CALENDAR ] && echo "main" || echo $BRANCH_MAUI_CALENDAR)

echo "Cloning Maui Calendar from branch $MAUI_CALENDAR_VERSION"

git clone https://invent.kde.org/maui/mauikit-calendar.git --depth=1 -b $MAUI_CALENDAR_VERSION

cd maui-calendar
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DBUNDLE_MAUI_STYLE=ON \
    .

make -j$(nproc)
DESTDIR=/ make install
make install
