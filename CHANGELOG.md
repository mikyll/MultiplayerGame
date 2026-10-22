# Changelog

All notable changes of each version, generated from the git history by `scripts/changelog.py`: don't edit it by hand.

## Versions

- [Unreleased](#Unreleased)
- [v0.7.0](#v0.7.0) (2026-10-22)
- [v0.6.0](#v0.6.0) (2026-10-21)
- [v0.5.0](#v0.5.0) (2026-10-21)
- [v0.4-beta](#v0.4-beta) (2024-04-25)
- [v0.3-beta](#v0.3-beta) (2024-04-21)
- [v0.2-beta](#v0.2-beta) (2024-04-20)
- [v0.1-beta](#v0.1-beta) (2024-04-20)

<a id="Unreleased"></a>

## Unreleased

[Compare with v0.7.0](https://github.com/mikyll/MultiplayerGame/compare/v0.7.0...HEAD)

### Build

- add script to generate the changelog from git history ([`bb43a02`](https://github.com/mikyll/MultiplayerGame/commit/bb43a02))


<a id="v0.7.0"></a>

## v0.7.0 (2026-10-22)

[Compare with v0.6.0](https://github.com/mikyll/MultiplayerGame/compare/v0.6.0...v0.7.0)

### Features

- **net**: refuse clients with a different protocol version ([`887fbb4`](https://github.com/mikyll/MultiplayerGame/commit/887fbb4))

### Bug Fixes

- **net**: size message buffers for CONNECT_OK with every player ([`b92168d`](https://github.com/mikyll/MultiplayerGame/commit/b92168d))

### Performance

- **net**: send state updates on an unreliable channel ([`57f97a0`](https://github.com/mikyll/MultiplayerGame/commit/57f97a0))

### Refactoring

- **net**: serialize network messages field by field instead of sending raw structs ([`86df45d`](https://github.com/mikyll/MultiplayerGame/commit/86df45d))
- **client**: move game protocol handling from the client to game/netsync ([`1a55284`](https://github.com/mikyll/MultiplayerGame/commit/1a55284))
- **server**: move game protocol handling from the server to game/netsync ([`36463dc`](https://github.com/mikyll/MultiplayerGame/commit/36463dc))

### Build

- bump project version to 0.7.0 ([`23e6daf`](https://github.com/mikyll/MultiplayerGame/commit/23e6daf))

### Tests

- add end-to-end network test with headless server, clients and probes ([`de720c4`](https://github.com/mikyll/MultiplayerGame/commit/de720c4))
- add CTest suite with player unit tests and a sanitized dev.sh test command ([`744f4cd`](https://github.com/mikyll/MultiplayerGame/commit/744f4cd))

### Documentation

- document tests, protocol versioning and roadmap in README ([`2ebab3e`](https://github.com/mikyll/MultiplayerGame/commit/2ebab3e))
- describe networking and netsync roles in README project structure ([`a62c50b`](https://github.com/mikyll/MultiplayerGame/commit/a62c50b))


<a id="v0.6.0"></a>

## v0.6.0 (2026-10-21)

[Compare with v0.5.0](https://github.com/mikyll/MultiplayerGame/compare/v0.5.0...v0.6.0)

### Refactoring

- move the project from MultiplayerGame_VisualStudio to the repository root ([`0e4e946`](https://github.com/mikyll/MultiplayerGame/commit/0e4e946))
- **game**: keep the local player reference in the game state ([`6fe3fde`](https://github.com/mikyll/MultiplayerGame/commit/6fe3fde))
- **client**: group per-session state into a ClientSession struct ([`1b14da5`](https://github.com/mikyll/MultiplayerGame/commit/1b14da5))
- **server**: group per-session state into a ServerSession struct ([`5afcbb0`](https://github.com/mikyll/MultiplayerGame/commit/5afcbb0))
- **player**: store players in a fixed array indexed by ID ([`7aaffd8`](https://github.com/mikyll/MultiplayerGame/commit/7aaffd8))

### Build

- keep container caches out of the source tree ([`2a7b0b1`](https://github.com/mikyll/MultiplayerGame/commit/2a7b0b1))
- remove Visual Studio project and makefiles in favor of CMake ([`ac2cac4`](https://github.com/mikyll/MultiplayerGame/commit/ac2cac4))
- add CMake build and use it in the container workflow ([`f376b86`](https://github.com/mikyll/MultiplayerGame/commit/f376b86))

### Documentation

- update README build instructions for CMake ([`573bafa`](https://github.com/mikyll/MultiplayerGame/commit/573bafa))


<a id="v0.5.0"></a>

## v0.5.0 (2026-10-21)

[Compare with v0.4-beta](https://github.com/mikyll/MultiplayerGame/compare/v0.4-beta...v0.5.0)

### Bug Fixes

- **init**: exit when window, renderer or ENet initialization fails ([`e75c2ac`](https://github.com/mikyll/MultiplayerGame/commit/e75c2ac))
- **player**: decrement players count when removing dead players ([`2b83965`](https://github.com/mikyll/MultiplayerGame/commit/2b83965))
- **player**: normalize diagonal movement speed ([`5024a48`](https://github.com/mikyll/MultiplayerGame/commit/5024a48))
- **main**: cap frame rate with an absolute deadline and a high resolution timer ([`5b44285`](https://github.com/mikyll/MultiplayerGame/commit/5b44285))
- **fps**: ignore zero frame times and always initialize the FPS label ([`29edeb6`](https://github.com/mikyll/MultiplayerGame/commit/29edeb6))
- **defs**: parenthesize MAX_SERVER_PEERS macro ([`05d835b`](https://github.com/mikyll/MultiplayerGame/commit/05d835b))
- **net**: handle host resolution failures in client and server ([`fbfcc1f`](https://github.com/mikyll/MultiplayerGame/commit/fbfcc1f))
- **utils**: reject empty or trailing segments in IP address validation ([`6bb85a2`](https://github.com/mikyll/MultiplayerGame/commit/6bb85a2))
- **server**: drop redundant peer reset and guard missing player on disconnect ([`03e8d30`](https://github.com/mikyll/MultiplayerGame/commit/03e8d30))
- **player**: compare local player by pointer to avoid NULL dereference ([`ca12508`](https://github.com/mikyll/MultiplayerGame/commit/ca12508))
- **player**: refuse to spawn players when no valid ID is available ([`32bfa1f`](https://github.com/mikyll/MultiplayerGame/commit/32bfa1f))
- **net**: reset host type whenever the host is gone ([`bd248d3`](https://github.com/mikyll/MultiplayerGame/commit/bd248d3))
- **client**: report failed connection attempts only through connectFailCallback ([`7a0abc9`](https://github.com/mikyll/MultiplayerGame/commit/7a0abc9))
- **ui**: consume only text input keys so RETURN reaches its handler ([`56bfa61`](https://github.com/mikyll/MultiplayerGame/commit/56bfa61))
- **net**: sync player color between server and clients ([`a89b652`](https://github.com/mikyll/MultiplayerGame/commit/a89b652))
- **client**: stop overwriting local player position with game state ([`1edf272`](https://github.com/mikyll/MultiplayerGame/commit/1edf272))
- **client**: raise connection timeout to 5 seconds ([`121961a`](https://github.com/mikyll/MultiplayerGame/commit/121961a))
- **server**: deliver CONNECT_DENIED when the server is full ([`dd537c7`](https://github.com/mikyll/MultiplayerGame/commit/dd537c7))
- **server**: destroy received packets after handling them ([`7bf95ee`](https://github.com/mikyll/MultiplayerGame/commit/7bf95ee))
- **server**: validate player state size and sender before applying it ([`2f9eed0`](https://github.com/mikyll/MultiplayerGame/commit/2f9eed0))
- **client**: validate size and players count of received messages ([`6ec30b3`](https://github.com/mikyll/MultiplayerGame/commit/6ec30b3))
- **text**: forward va_list through secure_vsprintf instead of a variadic call ([`e0b8eba`](https://github.com/mikyll/MultiplayerGame/commit/e0b8eba))
- **client**: stop servicing events after the client host is destroyed ([`310ad6a`](https://github.com/mikyll/MultiplayerGame/commit/310ad6a))
- **client**: clear server peer reference when destroying the client host ([`97d365f`](https://github.com/mikyll/MultiplayerGame/commit/97d365f))
- **player**: reset local player reference when its entity is freed ([`813da61`](https://github.com/mikyll/MultiplayerGame/commit/813da61))
- **server**: clear client peers when destroying the server host ([`97d5f3d`](https://github.com/mikyll/MultiplayerGame/commit/97d5f3d))
- **server**: avoid NULL dereference when removing a disconnected peer ([`3cb5e51`](https://github.com/mikyll/MultiplayerGame/commit/3cb5e51))

### Refactoring

- restructure sources into core, engine, game, ui, networking and utils ([`a422986`](https://github.com/mikyll/MultiplayerGame/commit/a422986))
- organize source files into folders by area ([`f15e2b5`](https://github.com/mikyll/MultiplayerGame/commit/f15e2b5))

### Build

- add containerized Linux build and run workflow ([`f94494d`](https://github.com/mikyll/MultiplayerGame/commit/f94494d))

### Documentation

- update README with how to play, controls and project structure ([`4be011b`](https://github.com/mikyll/MultiplayerGame/commit/4be011b))
- update README with Linux container build instructions ([`f97b87b`](https://github.com/mikyll/MultiplayerGame/commit/f97b87b))
- update README ([`1a195e2`](https://github.com/mikyll/MultiplayerGame/commit/1a195e2))

### Chores

- remove dead connection string, empty UI functions and unused DATA_DIR ([`aa4cd94`](https://github.com/mikyll/MultiplayerGame/commit/aa4cd94))


<a id="v0.4-beta"></a>

## v0.4-beta (2024-04-25)

[Compare with v0.3-beta](https://github.com/mikyll/MultiplayerGame/compare/v0.3-beta...v0.4-beta)

### Other changes

- update ([`111ce83`](https://github.com/mikyll/MultiplayerGame/commit/111ce83))
- Update README.md ([`60fc25a`](https://github.com/mikyll/MultiplayerGame/commit/60fc25a))
- update global structures ([`a1660d3`](https://github.com/mikyll/MultiplayerGame/commit/a1660d3))
- a lil cleaning ([`1e3501c`](https://github.com/mikyll/MultiplayerGame/commit/1e3501c))


<a id="v0.3-beta"></a>

## v0.3-beta (2024-04-21)

[Compare with v0.2-beta](https://github.com/mikyll/MultiplayerGame/compare/v0.2-beta...v0.3-beta)

### Other changes

- Update client.c ([`763fae0`](https://github.com/mikyll/MultiplayerGame/commit/763fae0))
- update ([`bc8afb8`](https://github.com/mikyll/MultiplayerGame/commit/bc8afb8))


<a id="v0.2-beta"></a>

## v0.2-beta (2024-04-20)

[Compare with v0.1-beta](https://github.com/mikyll/MultiplayerGame/compare/v0.1-beta...v0.2-beta)

### Other changes

- update ([`ffd82f4`](https://github.com/mikyll/MultiplayerGame/commit/ffd82f4))
- Update README.md ([`3e15165`](https://github.com/mikyll/MultiplayerGame/commit/3e15165))


<a id="v0.1-beta"></a>

## v0.1-beta (2024-04-20)

### Other changes

- Create demo.mp4 ([`4e04847`](https://github.com/mikyll/MultiplayerGame/commit/4e04847))
- Update README.md ([`61e250e`](https://github.com/mikyll/MultiplayerGame/commit/61e250e))
- Update README.md ([`027c96d`](https://github.com/mikyll/MultiplayerGame/commit/027c96d))
- update ([`ec760b2`](https://github.com/mikyll/MultiplayerGame/commit/ec760b2))
- Update README.md ([`276cb85`](https://github.com/mikyll/MultiplayerGame/commit/276cb85))
- update ([`12e8c28`](https://github.com/mikyll/MultiplayerGame/commit/12e8c28))
- update ([`c99ac46`](https://github.com/mikyll/MultiplayerGame/commit/c99ac46))
- update ([`604703c`](https://github.com/mikyll/MultiplayerGame/commit/604703c))
- update multiplayer game ([`04f9b4d`](https://github.com/mikyll/MultiplayerGame/commit/04f9b4d))
- refactor main functions ([`10b97ba`](https://github.com/mikyll/MultiplayerGame/commit/10b97ba))
- cleaning project ([`577d6ea`](https://github.com/mikyll/MultiplayerGame/commit/577d6ea))
- cleaning project ([`33c65e4`](https://github.com/mikyll/MultiplayerGame/commit/33c65e4))
- update ([`39dddb3`](https://github.com/mikyll/MultiplayerGame/commit/39dddb3))
- update broken multiplayer working ([`cb05637`](https://github.com/mikyll/MultiplayerGame/commit/cb05637))
- update repo stuff ([`8e4f204`](https://github.com/mikyll/MultiplayerGame/commit/8e4f204))
- Base Single Player Game ([`34aaa4b`](https://github.com/mikyll/MultiplayerGame/commit/34aaa4b))
