#!/usr/bin/env bash
# Build and run the game on Linux inside a container (requires only podman or docker + an X11/XWayland session).
#
# Usage: ./dev.sh <command>
#   image   build the container image
#   build   compile the game (builds the image first if missing)
#   run     compile and launch one game instance (run it in two terminals to play server + client)
#   shell   open a shell inside the container
#   clean   remove build artifacts
set -euo pipefail

IMAGE=multiplayer-game-dev
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if command -v podman >/dev/null 2>&1; then
    ENGINE=podman
    USER_ARGS=(--userns=keep-id)
elif command -v docker >/dev/null 2>&1; then
    ENGINE=docker
    USER_ARGS=(--user "$(id -u):$(id -g)")
else
    echo "Error: podman or docker is required." >&2
    exit 1
fi

image() {
    "$ENGINE" build -t "$IMAGE" "$ROOT"
}

ensure_image() {
    "$ENGINE" image inspect "$IMAGE" >/dev/null 2>&1 || image
}

container() {
    ensure_image

    local x11_args=()
    if [[ -n "${DISPLAY:-}" ]]; then
        local xauth="${XAUTHORITY:-$HOME/.Xauthority}"
        x11_args+=(-e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:ro)
        if [[ -f "$xauth" ]]; then
            x11_args+=(-e XAUTHORITY=/tmp/.Xauthority -v "$xauth:/tmp/.Xauthority:ro")
        fi
    fi

    local extra_args=()
    [[ -t 0 ]] && extra_args+=(-it)
    # GPU acceleration if available, otherwise Mesa falls back to software rendering
    [[ -e /dev/dri ]] && extra_args+=(--device /dev/dri)

    # Host network: multiple instances reach each other on localhost
    "$ENGINE" run --rm \
        --network host \
        "${USER_ARGS[@]}" \
        "${x11_args[@]}" \
        "${extra_args[@]}" \
        -v "$ROOT:/src:z" \
        "$IMAGE" "$@"
}

case "${1:-}" in
    image) image ;;
    build) container make ;;
    run)   container bash -c "make && ./multiplayer_game" ;;
    shell) container bash ;;
    clean) container make clean ;;
    *)     sed -n '4,10p' "$0" | sed 's/^# \{0,1\}//'; exit 1 ;;
esac
