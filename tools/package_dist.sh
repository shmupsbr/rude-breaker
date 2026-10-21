#!/bin/bash
# Build a player zip. The zip contains the program, README, and the ymfm
# license. It must not contain the Rude Breaker disk or the Yamaha ROM.
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
DIST="$ROOT/dist"
SDL_VER="${SDL_VER:-2.32.8}"
JOB="${1:-}"

stage_common() {
    local name="$1"
    local stage="$DIST/$name"
    rm -rf "$stage"
    mkdir -p "$stage" "$DIST"
    cp "$ROOT/README.md" "$stage/README.md"
    cp "$ROOT/third_party/ymfm/LICENSE" "$stage/LICENSE-ymfm.txt"
    printf '%s\n' "$stage"
}

reject_copyrighted() {
    local zip="$1"
    local hit
    hit=$(unzip -Z1 "$zip" | grep -E '(^|/)(SGM1\.DAT|FPLAY\.COM|SONG\.DAT|RBRANK\.DAT|SGC\.COM|READJS\.COM|RUDEBR\.BAT|ym2608_adpcm_rom\.bin|ym2608_rom.*\.zip)$' || true)
    if [[ -n "$hit" ]]; then
        echo "refusing to publish copyrighted data:" >&2
        echo "$hit" >&2
        exit 1
    fi
}

finish_zip() {
    local name="$1"
    local stage="$2"
    local zip="$DIST/${name}.zip"
    rm -f "$zip"
    (cd "$DIST" && zip -r -X "${name}.zip" "$name")
    reject_copyrighted "$zip"
    echo "wrote $zip"
    unzip -Z1 "$zip"
}

linux_static_prefix() {
    echo "${SDL_PREFIX:-/tmp/rudebr-sdl2-static}"
}

build_linux_sdl() {
    local prefix
    prefix=$(linux_static_prefix)
    if [[ -f "$prefix/lib/libSDL2.a" ]]; then
        return 0
    fi
    local src="/tmp/SDL2-${SDL_VER}"
    local tarball="/tmp/SDL2-${SDL_VER}.tar.gz"
    if [[ ! -f "$src/configure" ]]; then
        curl -fsSL -o "$tarball" \
            "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VER}/SDL2-${SDL_VER}.tar.gz"
        rm -rf "$src"
        tar -xzf "$tarball" -C /tmp
    fi
    mkdir -p "$src/build-static"
    (
        cd "$src/build-static"
        opts=(
            --prefix="$prefix"
            --disable-shared
            --enable-static
        )
        # Keep only switches this SDL release actually advertises.
        # X11 and ALSA stay on. Everything else that would drag a
        # session daemon into the binary is turned off when present.
        while IFS= read -r opt; do
            if ../configure --help | grep -F -q -- "$opt"; then
                opts+=("$opt")
            fi
        done <<'EOF'
--enable-video-x11
--enable-alsa
--disable-video-wayland
--disable-video-kmsdrm
--disable-video-opengl
--disable-video-opengles
--disable-video-opengles1
--disable-video-opengles2
--disable-video-vulkan
--disable-pulseaudio
--disable-pipewire
--disable-jack
--disable-sndio
--disable-esd
--disable-arts
--disable-nas
--disable-diskaudio
--disable-oss
--disable-video-rpi
--disable-libsamplerate
--disable-dbus
--disable-ime
--disable-fcitx
--disable-ibus
EOF
        ../configure "${opts[@]}"
        make -j"$(nproc)"
        make install
    )
}

package_linux() {
    local prefix stage pc libs
    build_linux_sdl
    prefix=$(linux_static_prefix)
    pc="$prefix/lib/pkgconfig/sdl2.pc"
    if [[ ! -f "$pc" ]]; then
        echo "static SDL2 pkg-config file missing: $pc" >&2
        exit 1
    fi
    # Force the static archive. X11 and ALSA stay dynamic, which is what
    # a desktop distro already has.
    # -lSDL2 is the static archive. -lSDL2main is not linked: this program
    # keeps its own main (SDL_MAIN_HANDLED).
    libs=""
    for tok in $(PKG_CONFIG_PATH="$prefix/lib/pkgconfig" pkg-config --libs --static sdl2); do
        if [[ "$tok" == "-lSDL2main" ]]; then
            continue
        fi
        if [[ "$tok" == "-lSDL2" ]]; then
            libs+=" -Wl,-Bstatic -lSDL2 -Wl,-Bdynamic"
        else
            libs+=" $tok"
        fi
    done
    make -C "$ROOT" BUILD=build/pkg-linux \
        SDL2_CFLAGS="$(PKG_CONFIG_PATH="$prefix/lib/pkgconfig" pkg-config --cflags sdl2)" \
        SDL2_LIBS="$libs -static-libgcc -static-libstdc++"
    stage=$(stage_common rudebreaker-linux-x86_64)
    cp "$ROOT/build/pkg-linux/rudebreaker" "$stage/rudebreaker"
    chmod +x "$stage/rudebreaker"
    if command -v strip >/dev/null 2>&1; then
        strip "$stage/rudebreaker" || true
    fi
    if ldd "$stage/rudebreaker" | grep -E 'libSDL2\.so'; then
        echo "Linux build still depends on libSDL2.so" >&2
        exit 1
    fi
    # X11 and ALSA are dlopen'd. Their sonames have to be compiled in
    # or the binary can only open the dummy driver.
    # grep -q would SIGPIPE strings under pipefail and look like a miss.
    if ! strings "$stage/rudebreaker" | grep -F 'libX11.so.6' >/dev/null; then
        echo "Linux build is missing the X11 video driver" >&2
        exit 1
    fi
    if ! strings "$stage/rudebreaker" | grep -F 'libasound.so.2' >/dev/null; then
        echo "Linux build is missing the ALSA audio driver" >&2
        exit 1
    fi
    finish_zip rudebreaker-linux-x86_64 "$stage"
}

mingw_root() {
    echo "${MINGW_SDL:-/tmp/sdl2-mingw-${SDL_VER}}"
}

fetch_mingw_sdl() {
    local root
    root=$(mingw_root)
    if [[ -f "$root/x86_64-w64-mingw32/bin/SDL2.dll" ]]; then
        return 0
    fi
    local tarball="/tmp/SDL2-devel-${SDL_VER}-mingw.tar.gz"
    curl -fsSL -o "$tarball" \
        "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VER}/SDL2-devel-${SDL_VER}-mingw.tar.gz"
    rm -rf "$root"
    mkdir -p "$root"
    tar -xzf "$tarball" -C "$root" --strip-components=1
}

package_windows() {
    local root inc lib stage
    command -v x86_64-w64-mingw32-gcc >/dev/null
    command -v x86_64-w64-mingw32-g++ >/dev/null
    fetch_mingw_sdl
    root=$(mingw_root)
    inc="$root/x86_64-w64-mingw32/include/SDL2"
    lib="$root/x86_64-w64-mingw32/lib"
    if [[ ! -f "$root/x86_64-w64-mingw32/bin/SDL2.dll" ]]; then
        echo "SDL2.dll not found under $root" >&2
        find "$root" -name 'SDL2.dll' -o -name 'sdl2.pc' | head
        exit 1
    fi
    # Import library name varies. Prefer the dll import lib over a static one.
    local link="-L$lib -lSDL2"
    if [[ -f "$lib/libSDL2.dll.a" ]]; then
        link="-L$lib -lSDL2"
    fi
    make -C "$ROOT" BUILD=build/pkg-mingw EXE=.exe \
        CC=x86_64-w64-mingw32-gcc \
        CXX=x86_64-w64-mingw32-g++ \
        SDL2_CFLAGS="-I$inc" \
        SDL2_LIBS="$link -mconsole -static-libgcc -static-libstdc++"
    stage=$(stage_common rudebreaker-windows-x86_64)
    cp "$ROOT/build/pkg-mingw/rudebreaker.exe" "$stage/rudebreaker.exe"
    cp "$root/x86_64-w64-mingw32/bin/SDL2.dll" "$stage/SDL2.dll"
    if command -v x86_64-w64-mingw32-strip >/dev/null 2>&1; then
        x86_64-w64-mingw32-strip "$stage/rudebreaker.exe" || true
    fi
    finish_zip rudebreaker-windows-x86_64 "$stage"
}

package_macos() {
    local stage dylib id
    if ! command -v brew >/dev/null 2>&1; then
        echo "macos packaging needs Homebrew SDL2" >&2
        exit 1
    fi
    brew install sdl2
    local prefix
    prefix=$(brew --prefix sdl2)
    export PKG_CONFIG_PATH="$prefix/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
    make -C "$ROOT" BUILD=build/pkg-macos \
        SDL2_CFLAGS="$(pkg-config --cflags sdl2)" \
        SDL2_LIBS="$(pkg-config --libs sdl2)"
    stage=$(stage_common "rudebreaker-macos-$(uname -m)")
    cp "$ROOT/build/pkg-macos/rudebreaker" "$stage/rudebreaker"
    chmod +x "$stage/rudebreaker"
    dylib=$(otool -L "$stage/rudebreaker" | awk '/libSDL2/{print $1; exit}')
    if [[ -z "$dylib" ]]; then
        echo "could not find libSDL2 in the macOS binary" >&2
        otool -L "$stage/rudebreaker" >&2
        exit 1
    fi
    cp "$dylib" "$stage/libSDL2.dylib"
    install_name_tool -id "@executable_path/libSDL2.dylib" "$stage/libSDL2.dylib"
    install_name_tool -change "$dylib" "@executable_path/libSDL2.dylib" "$stage/rudebreaker"
    # Drop any leftover absolute SDL id inside the copied library.
    id=$(otool -D "$stage/libSDL2.dylib" | tail -n 1)
    if [[ "$id" != "@executable_path/libSDL2.dylib" ]]; then
        install_name_tool -id "@executable_path/libSDL2.dylib" "$stage/libSDL2.dylib"
    fi
    if command -v strip >/dev/null 2>&1; then
        strip "$stage/rudebreaker" || true
    fi
    finish_zip "rudebreaker-macos-$(uname -m)" "$stage"
}

case "$JOB" in
    linux) package_linux ;;
    windows) package_windows ;;
    macos) package_macos ;;
    *)
        echo "usage: $0 linux|windows|macos" >&2
        exit 2
        ;;
esac
