#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
ensure_checkout maui-core https://github.com/Nitrux/maui-core.git "${BRANCH_MAUICORE:-main}"
apply_patch maui-core "$REPO_ROOT/scripts/patches/maui-core-fixes.patch"
build_component maui-core https://github.com/Nitrux/maui-core.git "${BRANCH_MAUICORE:-main}"
