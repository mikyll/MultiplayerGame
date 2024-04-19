<div align="center">

  [![MIT License][license-shield]][license-url]
  [![Size][size-shield]][size-url]
  [![Issues][issues-shield]][issues-url]
  [![Downloads][downloads-shield]][downloads-url]
  [![Stars][stars-shield]][stars-url]\
  [![C][c-shield]][c-url]
  [![VS][vs-shield]][vs-url]

# MultiplayerGame
A super simple multiplayer game made in C, with [SDL2](https://github.com/libsdl-org/SDL) and [ENet](https://github.com/lsalzman/enet).

</div>

## Demo

https://github.com/mikyll/MultiplayerGame/assets/56556806/f9213bac-d907-4bee-a7e5-a9922a075f32

## Features/Roadmap

- [x] deltaTime game loop with the possibility to cap to a specific framerate;
- [x] multiplayer based on a server/client model and UDP reliable packets;
- [ ] server->client disconnection;
- [ ] non-blocking wait for client connections;
- [ ] improve UI elements and refactor the related code;

## Project Setup

TODO

### SDL2 Libraries

### ENet Library

Related issue: https://github.com/lsalzman/enet/issues/80 (Useful for installation)

## Build

The project is developed on Visual Studio 2022 (MSVC v14.39.33519), with the following dependencies:
- [SDL v2.30.2](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2);
- [SDL_image v2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2);
- [ENet v1.3.17](https://github.com/lsalzman/enet/releases/tag/v1.3.17).

### Windows

You can use the Microsoft Build Engine, from within the Visual Studio Command Prompt:
```ps
msbuild MultiplayerGame_VisualStudio/MultiplayerGame.sln
```
Or just open the solution and build the project.

The build will generate an executable file named **`MultiplayerGame.exe`** in `MultiplayerGame_VisualStudio/x64/Debug` or `MultiplayerGame_VisualStudio/x64/Release` directories.\
**NB**: to run the program you need SDL2.dll and SDL2_image.dll.

### Linux

1. You must first install sdl2 and enet development libraries:
  ```bash
  sudo apt update
  sudo apt install libsdl2-dev libsdl2-image-dev libenet-dev
  ```
2. Then you can just compile using the makefile:
  ```bash
  make
  ```
This will generate an executable file named **`multiplayer_game`**.

## Aknowledgements

Huge thanks to [Parallel Realities](https://www.parallelrealities.co.uk/) and their fenomenal SDL2 tutorials. Go check them out on [their website](https://www.parallelrealities.co.uk/tutorials/) and on [Itch.io](https://parallelrealities.itch.io/), the code bundles are extremely cheap and they are insanely well made.

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
[stars-shield]: https://custom-icon-badges.herokuapp.com/github/stars/mikyll/MultiplayerGame?logo=star&logoColor=yellow&style=flat
[stars-url]: https://github.com/mikyll/MultiplayerGame/stargazers

[c-shield]: https://img.shields.io/badge/C-%2300599C.svg?logo=c&logoColor=white
[c-url]: https://www.open-std.org/jtc1/sc22/wg14/
[vs-shield]: https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?logo=visual-studio&logoColor=white
[vs-url]: https://code.visualstudio.com/

[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?logo=linkedin&colorB=0077B5
[linkedin-url]: https://www.linkedin.com/in/michele-righi/?locale=en_US
[github-shield]: https://img.shields.io/github/followers/mikyll.svg?style=social&label=Follow
[github-url]: https://github.com/mikyll
