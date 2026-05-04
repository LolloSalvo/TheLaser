# The Laser

A 2D puzzle game written in C++ with SDL2. Laser, mirrors, a 12×12 grid, and a 60-second timer. Place mirrors to bounce the beam and hit the target before time runs out. Each solved level adds +10 seconds and increases your score.

School project — Lorenzo Salvini, class 1DAE14 (Howest – Digital Arts & Entertainment).

## Stack

- **Language**: C++17
- **Build system**: Visual Studio 2022 (Windows) or CMake ≥ 3.16 (Linux / macOS)
- **Libraries** (bundled under `Libraries/` for Windows; installed via apt/brew on Linux):
  - SDL2 2.30.x
  - SDL2_image 2.8.x
  - SDL2_mixer 2.8.x
  - SDL2_ttf 2.2x.x
- **Platform**: Windows 10/11 (x64/Win32), Ubuntu 22.04+, and other Linux distros

## Project structure

```
TheLaser/
├── CMakeLists.txt             # CMake root (Linux / cross-platform build)
├── CodePrototype.sln          # Visual Studio solution (Windows)
├── Engine/                    # Static lib: BaseGame, Texture, utils, SVGParser, Vector2f, Matrix2x3, audio
├── TheLaser/                  # Executable: Game, Grid, Mirror, Laser, LevelGenerator, Digit, Cell
│   └── Resources/             # digits.png, numbers.png, TypoDigit.otf
└── Libraries/                 # Pre-built SDL2 headers + .lib + .dll (Windows only)
```

`TheLaser` depends on the `Engine` static lib. A post-build step copies the DLLs and the contents of `Resources/` next to the executable.

## Install & build

### Windows (Visual Studio 2022)

**Requirements:** Visual Studio 2022 with the *"Desktop development with C++"* workload, Windows 10 SDK, MSVC toolset `v143`.

1. Clone or extract the repo into a path without exotic characters.
2. Open `TheLaser/CodePrototype.sln` in Visual Studio 2022.
3. Set `TheLaser` as the **Startup Project** (right-click → *Set as Startup Project*).
4. Pick configuration **Debug** or **Release**, platform **x64** (recommended) or **Win32**.
5. Build with `Ctrl+Shift+B`. Run with `F5`.

SDL2 DLLs and `Resources/` are copied automatically into the output folder on first build.

**Command line** (Developer Prompt for VS 2022):
```bat
msbuild CodePrototype.sln /p:Configuration=Release /p:Platform=x64
```
Output: `x64\Release\TheLaser.exe`.

### Ubuntu / Linux (CMake)

**Requirements:** GCC/Clang, CMake ≥ 3.16, SDL2 dev libraries.

```bash
sudo apt install build-essential cmake \
    libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
    libgl1-mesa-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/TheLaser/TheLaser
```

`Resources/` (fonts and sprites) are copied next to the executable automatically by CMake.

## Controls

- **Mouse**: place / rotate mirrors on the grid
- **Keyboard**: handled in `Game::ProcessKeyDownEvent` (see `TheLaser/Game.cpp`)
- Goal: hit the target with the laser beam before the timer hits zero

## Troubleshooting

- **`SDL2.dll` not found at launch**: run the executable from `x64\Debug` or `x64\Release` (working dir = `$(TargetDir)`). DLLs are placed there by the post-build step.
- **Missing resources** (`digits.png`, fonts, ...): rebuild — the post-build event copies everything from `TheLaser/Resources/`.
- **Linker error on `Engine.lib`**: build the `Engine` project first, or build the whole solution.
- **Memory leak output on exit (Debug)**: expected. It comes from `_CrtDumpMemoryLeaks` and is diagnostic only.

## License

No license specified for the game code. SDL2 and its modules remain under their original zlib license.
