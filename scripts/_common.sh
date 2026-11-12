#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WORKDIR="${WORKDIR:-$(dirname "$REPO_ROOT")}"
PREFIX="${PREFIX:-/usr}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc)}"
BUILD_ROOT="${BUILD_ROOT:-$WORKDIR/.build}"

SUDO=""
if [ "$(id -u)" -ne 0 ]; then SUDO="sudo"; fi

configure_build_install() {
    local name="$1" src="$2"; shift 2
    local bld="$BUILD_ROOT/$name"

    [ -f "$src/CMakeLists.txt" ] || { echo "ERROR: $src/CMakeLists.txt not found" >&2; exit 1; }

    rm -rf "$src/CMakeCache.txt" "$src/CMakeFiles"
    if [ "${CLEAN:-0}" = "1" ]; then rm -rf "$bld"; fi

    echo "==> [$name] configure"
    cmake -S "$src" -B "$bld" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX="$PREFIX" \
        "$@"
    echo "==> [$name] build"
    cmake --build "$bld" -j"$JOBS"
    echo "==> [$name] install -> $PREFIX"
    $SUDO cmake --install "$bld"
    $SUDO ldconfig || true
}

ensure_checkout() {
    local name="$1" url="$2" branch="$3"
    local src="$WORKDIR/$name"

    if [ -d "$src/.git" ]; then
        echo "==> [$name] using existing checkout $src (not re-cloned)"
    elif [ -e "$src" ]; then
        echo "ERROR: $src exists but is not a git checkout" >&2; exit 1
    else
        echo "==> [$name] cloning $url ($branch)"
        git clone --depth=1 -b "$branch" "$url" "$src"
    fi
}

apply_patch() {
    local name="$1" patch="$2" src="$WORKDIR/$1"
    if git -C "$src" apply --reverse --check "$patch" >/dev/null 2>&1; then
        echo "==> [$name] patch already applied: $(basename "$patch")"
    elif git -C "$src" apply --check "$patch" >/dev/null 2>&1; then
        git -C "$src" apply "$patch"
        echo "==> [$name] applied patch: $(basename "$patch")"
    else
        echo "!!  [$name] WARNING: $(basename "$patch") does not apply cleanly to $src" >&2
        echo "!!  (upstream may have changed, or you patched it already) - continuing without it" >&2
    fi
}

build_component() {
    local name="$1" url="$2" branch="$3"; shift 3
    ensure_checkout "$name" "$url" "$branch"
    configure_build_install "$name" "$WORKDIR/$name" "$@"
}
