<div align="center">

  [![MIT License][license-shield]][license-url]
  [![Size][size-shield]][size-url]
  [![Issues][issues-shield]][issues-url]
  [![Downloads][downloads-shield]][downloads-url]
  [![Stars][stars-shield]][stars-url]\
  [![C][c-shield]][c-url]
  [![VS][vs-shield]][vs-url]

# MultiplayerGame

A super simple multiplayer "game" made in C, with [SDL2](https://github.com/libsdl-org/SDL) and [ENet](https://github.com/lsalzman/enet).

</div>

## Demo

https://github.com/mikyll/MultiplayerGame/assets/56556806/f9213bac-d907-4bee-a7e5-a9922a075f32

## Features/Roadmap

- [x] deltaTime game loop with the possibility to cap to a specific framerate;
- [x] multiplayer based on a server/client model and UDP reliable packets;
- [x] server->client disconnection;
- [x] non-blocking wait for client connections;
- [x] "server full" notification for clients exceeding the player limit;
- [x] position and color sync between players;
- [x] Linux build and run in a container;
- [ ] improve UI elements;
- [ ] refactor: CMake build, session lifecycle, network protocol with explicit serialization;

## How to Play

1. Start the game and press **Server**: you join the session as player `P0`.
2. Start other instances and press **Client** to connect (up to 4 players in total, server included).
   Before pressing **Client**, you can type the server IP address in the field at the top; leave it empty to connect to `localhost`.
   The server listens on UDP port `8987`.

### Controls

| Key | Action |
|---|---|
| <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> | Move |
| <kbd>Shift</kbd> / <kbd>Ctrl</kbd> (hold) | Move faster / slower |
| <kbd>Q</kbd> | Change color |
| <kbd>Esc</kbd> | Leave the session (on the server, it closes the session for everyone) |
| <kbd>0</kbd>-<kbd>9</kbd> <kbd>.</kbd> <kbd>Backspace</kbd> | Edit the IP address in the menu |

## Project Structure

```
MultiplayerGame_VisualStudio/
├── src/
│   ├── main.c        game loop
│   ├── core/         shared definitions and structures
│   ├── engine/       SDL setup, input, drawing, text
│   ├── game/         game and menu scenes, players
│   ├── ui/           UI widgets (text input, FPS counter)
│   ├── networking/   ENet host abstraction, client and server
│   └── utils/        string and validation helpers
├── resources/        fonts
└── libs/             SDL2, SDL2_image and ENet headers and x64 libraries (Windows)
```

## Build

The project is developed on Visual Studio 2022 (MSVC v14.39.33519), with the following dependencies:
- [SDL v2.30.2](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2);
- [SDL_image v2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2);
- [ENet v1.3.17](https://github.com/lsalzman/enet/releases/tag/v1.3.17).

### Windows

Headers and x64 import libraries are already included in `MultiplayerGame_VisualStudio/libs`; only the **x64** configurations are currently supported.
For ENet installation details, see [lsalzman/enet#80](https://github.com/lsalzman/enet/issues/80).

You can use the Microsoft Build Engine, from within the Visual Studio Command Prompt:
```ps
msbuild MultiplayerGame_VisualStudio/MultiplayerGame.sln
```
Or just open the solution and build the project.

The build will generate an executable file named **`MultiplayerGame.exe`** in `MultiplayerGame_VisualStudio/x64/Debug` or `MultiplayerGame_VisualStudio/x64/Release` directories.\
**NB**: to run the program you need `SDL2.dll` and `SDL2_image.dll` (from the SDL and SDL_image release packages) next to the executable.

### Linux

#### Container (recommended)

The only requirement is [Podman](https://podman.io/) or [Docker](https://www.docker.com/) and an X11 (or XWayland) session; toolchain and libraries live in the container image:
```bash
./dev.sh build   # compile (builds the image on first use)
./dev.sh run     # compile and launch the game
./dev.sh shell   # shell inside the build environment
./dev.sh clean   # remove build artifacts
```
To test multiplayer, run `./dev.sh run` in two terminals: pick **Server** in one and **Client** in the other.

#### Native

1. You must first install sdl2 and enet development libraries:
  ```bash
  sudo apt update
  sudo apt install libsdl2-dev libsdl2-image-dev libenet-dev
  ```
2. Then you can just compile using the makefile:
  ```bash
  cd MultiplayerGame_VisualStudio
  make
  ```
This will generate an executable file named **`multiplayer_game`**; run it from the same directory, so it can find `resources/`.

## Documentation

- [SDL2](https://wiki.libsdl.org/SDL2/FrontPage)
- [ENet](http://sauerbraten.org/enet/index.html)

## Acknowledgements

Huge thanks to [Parallel Realities](https://www.parallelrealities.co.uk/) and their phenomenal SDL2 tutorials. Go check them out on [their website](https://www.parallelrealities.co.uk/tutorials/) and on [Itch.io](https://parallelrealities.itch.io/), the code bundles are extremely cheap and they are insanely well made.

<div align="center">

  [![LinkedIn][linkedin-shield]][linkedin-url]
  [![GitHub followers][github-shield]][github-url]

</div>

[downloads-shield]: https://img.shields.io/github/downloads/mikyll/MultiplayerGame/total
[downloads-url]: https://github.com/mikyll/MultiplayerGame/releases/latest
[license-shield]: https://img.shields.io/github/license/mikyll/MultiplayerGame
[license-url]: https://github.com/mikyll/MultiplayerGame/blob/main/LICENSE
[size-shield]: 	https://img.shields.io/github/repo-size/mikyll/MultiplayerGame
[size-url]: https://github.com/mikyll/MultiplayerGame
[issues-shield]: https://img.shields.io/github/issues/mikyll/MultiplayerGame
[issues-url]: https://github.com/mikyll/MultiplayerGame/issues
[stars-shield]: https://custom-icon-badges.herokuapp.com/github/stars/mikyll/MultiplayerGame?style=flat
[stars-url]: https://github.com/mikyll/MultiplayerGame/stargazers

[c-shield]: https://img.shields.io/badge/C-%2300599C.svg?logo=c&logoColor=white
[c-url]: https://www.open-std.org/jtc1/sc22/wg14/
[vs-shield]: https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?logo=visual-studio&logoColor=white
[vs-url]: https://visualstudio.microsoft.com/

[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?logo=linkedin&colorB=0077B5
[linkedin-url]: https://www.linkedin.com/in/michele-righi/?locale=en_US
[github-shield]: https://img.shields.io/github/followers/mikyll.svg?style=social&label=Follow
[github-url]: https://github.com/mikyll
