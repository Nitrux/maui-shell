#!/bin/bash

mkdir -p maui-shell && cd maui-shell
cmake -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    ../../

make -j$(nproc)
make install