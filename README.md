# The Laser

A 2D puzzle game written in C++ with SDL2. Laser, mirrors, a 12×12 grid, and a 60-second timer. Place mirrors to bounce the beam and hit the target before time runs out. Each solved level adds +10 seconds and increases your score.

School project — Lorenzo Salvini, class 1DAE14 (Howest – Digital Arts & Entertainment).

## Stack

- **Language**: C++17, MSVC (`v143`)
- **Build system**: Visual Studio 2022 (.sln + .vcxproj)
- **Libraries** (already bundled under `Libraries/`):
  - SDL2 2.30.11
  - SDL2_image 2.8.4
  - SDL2_mixer 2.8.0
  - SDL2_ttf 2.24.0
- **Platform**: Windows 10/11, x64 or Win32

## Project structure

```
TheLaser/
├── CodePrototype.sln          # Visual Studio solution
├── Engine/                    # Static lib: BaseGame, Texture, utils, SVGParser, Vector2f, Matrix2x3, audio
├── TheLaser/                  # Executable: Game, Grid, Mirror, Laser, LevelGenerator, Digit, Cell
│   └── Resources/             # digits.png, numbers.png, TypoDigit.otf
└── Libraries/                 # Headers + .lib + .dll for SDL2 and submodules
```

`TheLaser` depends on the `Engine` static lib. A post-build step copies the DLLs and the contents of `Resources/` into `$(TargetDir)`.

## Requirements

- Windows 10/11
- Visual Studio 2022 with the **"Desktop development with C++"** workload
- Windows 10 SDK
- MSVC toolset `v143`

## Install & build

1. Clone or extract the repo into a path without exotic characters.
2. Open `TheLaser/CodePrototype.sln` in Visual Studio 2022.
3. Set `TheLaser` as the **Startup Project** (right-click the project → *Set as Startup Project*).
4. Pick configuration **Debug** or **Release**, platform **x64** (recommended) or **Win32**.
5. Build with `Ctrl+Shift+B`. Run with `F5`.

On the first build, SDL2 DLLs and the files under `Resources/` are copied automatically into the output folder (`x64/Debug/`, `x64/Release/`, etc.).

### Command line (Developer Prompt for VS 2022)

```bat
msbuild CodePrototype.sln /p:Configuration=Release /p:Platform=x64
```

Output executable: `x64\Release\TheLaser.exe`.

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
