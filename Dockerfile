# Build & run environment for the Linux version of the game.
# The source is mounted at runtime (see dev.sh), so the image only holds the toolchain and libraries.
FROM docker.io/library/ubuntu:24.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        pkg-config \
        libsdl2-dev \
        libsdl2-image-dev \
        libenet-dev \
        libgl1-mesa-dri \
    && rm -rf /var/lib/apt/lists/*

# No sound card inside the container: without this SDL_Init(SDL_INIT_EVERYTHING) fails
ENV SDL_AUDIODRIVER=dummy
# Keep runtime caches (e.g. Mesa shaders) out of the mounted source tree
ENV XDG_CACHE_HOME=/tmp/.cache

WORKDIR /src
