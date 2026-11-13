#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/_common.sh"
build_component cask-server https://github.com/Nitrux/cask-server.git "${BRANCH_CASK_SERVER:-main}"
