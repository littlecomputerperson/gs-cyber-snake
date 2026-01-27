# Cyber Snake

A modern reimagining of the classic Snake game built with OpenGL and cross-platform support using SDL2.

## Description

Cyber Snake is a fast-paced arcade game where players control a snake that grows longer as it consumes rats. A tribute to the classic MS-DOS QBasic game Nibbles, this modern reimagining features two distinct gameplay modes:

- **Classic Mode**: Traditional snake gameplay with static rats
- **Remix Mode**: Enhanced difficulty with moving rats that try to evade the snake

The game includes multiple levels across different worlds, each increasing in difficulty with faster snake movement and more challenging obstacles. Players can compete for high scores, customize game settings, and enjoy smooth OpenGL-powered graphics with scaling support for various screen resolutions.

## Tech Stack

### Core Technologies
- **C++11**: Primary programming language
- **OpenGL 2.1**: Graphics rendering (fixed-function pipeline)
- **SDL2**: Cross-platform window management, input handling, and event processing
- **SDL2_mixer**: Audio playback for music and sound effects

### Build System
- **CMake 3.10+**: Cross-platform build configuration
- **Platform-specific**: Visual Studio (Windows), Make/Ninja (Linux/macOS)

### Platform Abstraction Layer
Custom abstraction layer (`gs_platform.*`) provides seamless compatibility between Windows API and SDL2, allowing the codebase to compile on both platforms with minimal changes.

## File Structure

### Core Application Files
```
gs_app.cpp/h              - Main application framework and window management
gs_main.cpp/h             - Entry point for Windows (WinMain) and SDL (main)
gs_snake.cpp/h            - Game-specific logic, menus, and game states
gs_platform.cpp/h         - Platform abstraction layer (Windows/SDL2)
```

### Game System Library (GS)
```
gs_error.cpp/h            - Error reporting and logging
gs_file.cpp/h             - File I/O operations with path normalization
gs_ini_file.cpp/h         - INI file parsing for settings and high scores
gs_keyboard.cpp/h         - Keyboard input handling and mapping
gs_mouse.cpp/h            - Mouse input and cursor management
gs_object.cpp/h           - Base object class for all GS objects
gs_timer.cpp/h            - Frame timing and performance monitoring
```

### OpenGL Rendering System
```
gs_ogl_collide.cpp/h      - 2D collision detection utilities
gs_ogl_display.cpp/h      - OpenGL context management and rendering setup
gs_ogl_font.cpp/h         - Bitmap font rendering system
gs_ogl_image.cpp/h        - Image loading (TGA format)
gs_ogl_map.cpp/h          - Tile-based map rendering
gs_ogl_menu.cpp/h         - Interactive menu system
gs_ogl_particle.cpp/h     - Particle effects system
gs_ogl_sprite.cpp/h       - Basic sprite rendering
gs_ogl_sprite_ex.cpp/h    - Extended sprite with animation support
gs_ogl_texture.cpp/h      - Texture loading and management
```

### Audio System
```
gs_sdl_mixer_sound.cpp/h  - SDL2_mixer audio implementation
gs_fmod_sound.cpp/h       - FMOD audio implementation (legacy)
```

### Game Objects
```
snake.cpp/h               - Snake entity logic and rendering
rat.cpp/h                 - Rat entity logic and AI
level.cpp/h               - Level loading and obstacle management
```

### Build Configuration
```
CMakeLists.txt            - CMake build configuration
build.sh                  - Linux/macOS build script
build_windows.bat         - Windows build script (to be created)
build_macos.sh            - macOS-specific build script (to be created)
```

### Resources
```
gs_resource.h             - Resource ID definitions
data/                     - Game assets (textures, audio, levels)
settings.ini              - User settings (display, audio, controls)
hiscores.ini              - High score database
```

## Platform Support

### Windows

#### Prerequisites (Visual Studio)
- Visual Studio 2017 or later (with C++ desktop development tools)
- CMake 3.10 or later
- SDL2 development libraries
- SDL2_mixer development libraries

#### Building with Visual Studio
1. Install dependencies using vcpkg:
```cmd
vcpkg install sdl2:x64-windows sdl2-mixer:x64-windows
```

2. Run the build script:
```cmd
build_windows.bat
```

Or manually:
```cmd
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

3. Run the game:
```cmd
build\Release\CyberSnake.exe
```

#### Prerequisites (MinGW/GCC)
- MinGW-w64 or MSYS2 with GCC
- CMake 3.10 or later
- SDL2 development libraries
- SDL2_mixer development libraries

#### Building with MinGW/GCC

**Using MSYS2 (Recommended)**:

1. Install MSYS2 from https://www.msys2.org/

2. Open MSYS2 MinGW 64-bit terminal and install dependencies:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-make
```

3. Navigate to project directory and build:
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j$(nproc)
```

4. Run the game:
```bash
./CyberSnake.exe
```

**Using MinGW-w64 standalone**:

1. Download and install MinGW-w64 from https://mingw-w64.org/

2. Download SDL2 development libraries:
   - SDL2: https://www.libsdl.org/download-2.0.php (mingw version)
   - SDL2_mixer: https://www.libsdl.org/projects/SDL_mixer/ (mingw version)

3. Extract SDL2 libraries to MinGW installation (e.g., `C:\mingw64`)
   - Copy `include/SDL2/*` to `C:\mingw64\include\SDL2\`
   - Copy `lib/*` to `C:\mingw64\lib\`
   - Copy `bin/*.dll` to `C:\mingw64\bin\`

4. Build the project:
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j4
```

5. Copy required DLLs to build directory:
```cmd
copy C:\mingw64\bin\SDL2.dll .
copy C:\mingw64\bin\SDL2_mixer.dll .
```

6. Run the game:
```cmd
CyberSnake.exe
```

### Linux

#### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-mixer-dev libgl1-mesa-dev libglu1-mesa-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake SDL2-devel SDL2_mixer-devel mesa-libGL-devel mesa-libGLU-devel

# Arch Linux
sudo pacman -S base-devel cmake sdl2 sdl2_mixer mesa
```

#### Building
```bash
chmod +x build.sh
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Running
```bash
./build/CyberSnake
```

### macOS

#### Prerequisites
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake pkg-config sdl2 sdl2_mixer
```

#### Building
```bash
chmod +x build_macos.sh
./build_macos.sh
```

Or manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
cd ..
```

#### Running
```bash
open build/CyberSnake.app
```

Or from Finder, navigate to the `build` directory and double-click `CyberSnake.app`.

## Future Platform Support

### Android
**Feasibility**: High

The game's SDL2 foundation makes Android porting straightforward:
- SDL2 has excellent Android support with touch input mapping
- OpenGL ES 2.0 compatibility layer needed (current code uses OpenGL 2.1)
- File I/O needs adaptation for Android's asset system
- Virtual controls or gesture-based input for touch screens

**Required Changes**:
1. Create Android.mk/CMakeLists.txt for Android NDK
2. Implement touch input in `gs_keyboard.cpp` and `gs_mouse.cpp`
3. Convert OpenGL calls to OpenGL ES 2.0
4. Package assets in APK format
5. Add on-screen controls overlay

### iOS
**Feasibility**: High

Similar to Android, SDL2's iOS support is mature:
- SDL2 provides UIKit integration
- Metal backend available for better performance
- Touch input already supported by SDL2
- App Store compliance for audio/video formats

**Required Changes**:
1. Create Xcode project with SDL2 frameworks
2. Convert OpenGL to OpenGL ES or Metal
3. Implement iOS-specific file paths
4. Add touch controls
5. Handle app lifecycle (pause/resume)
6. Update audio format compatibility

### WebAssembly
**Feasibility**: Medium-High

Using Emscripten to compile to WebAssembly:
- SDL2 has Emscripten support
- OpenGL translates to WebGL
- File I/O needs virtual filesystem
- Audio through Web Audio API

**Required Changes**:
1. Set up Emscripten toolchain
2. Modify file loading for Emscripten's virtual FS
3. Adjust main loop for browser event model
4. Preload assets or implement progressive loading
5. Handle browser-specific input events
6. Optimize asset sizes for web delivery

**Build Command Example**:
```bash
emcc -o cybersnake.html $(sources) \
  -s USE_SDL=2 -s USE_SDL_MIXER=2 \
  -s WASM=1 --preload-file data
```

## Controls

### Keyboard
- **Arrow Keys**: Move snake (Up, Down, Left, Right)
- **Enter**: Select menu option / Confirm
- **Escape**: Pause game / Return to menu
- **P**: Pause/Resume game
- **M**: Mute/Unmute music
- **S**: Mute/Unmute sound effects
- **Alt+Enter**: Toggle fullscreen (Windows only)
- **Alt+X**: Quit application (Windows only)

### Mouse
- Navigate menus by hovering and clicking
- Cursor visible in menus, hidden during gameplay

## Configuration

### Display Settings (settings.ini)
- **DisplayWidth/Height**: Screen resolution
- **WindowMode**: Fullscreen (0) or Windowed (1)
- **EnableVSync**: Vertical sync on/off
- **FrameCap**: Maximum FPS (0 for unlimited)

### Audio Settings
- **MusicVolume**: 0-255
- **EffectsVolume**: 0-255

## Development

### Adding New Levels
Levels are stored in `data/` directory. Create new level files following the existing format with:
- Snake spawn position
- Rat spawn areas
- Obstacle layouts

### Extending Game Modes
Modify `gs_snake.cpp` to add new game modes:
1. Define mode constant in header
2. Add menu option in `PlayIntro()`
3. Implement mode-specific logic in `PlayGame()`

### Custom Rendering
The OpenGL rendering system is modular:
- Sprites: `gs_ogl_sprite.cpp`
- Fonts: `gs_ogl_font.cpp`
- Particles: `gs_ogl_particle.cpp`

## License

This project uses libraries with the following licenses:
- **SDL2**: zlib License
- **SDL2_mixer**: zlib License
- **OpenGL**: Various (hardware vendor implementations)

Game code and assets: Check individual file headers for copyright information.

## Troubleshooting

### Linux: "Failed to initialize SDL"
Ensure SDL2 development packages are installed and X11/Wayland is running.

### macOS: "Library not loaded"
Run `install_name_tool` to fix library paths or use `@rpath` in CMake.

### Windows: "Cannot find SDL2.dll"
Copy SDL2.dll and SDL2_mixer.dll to the executable directory.

### Performance Issues
- Disable VSync if experiencing lag
- Reduce resolution in settings
- Check GPU drivers are up to date

## Credits

- **Development**: Little Computer Person
- **Title Music**: "Stomp Rock for Champions" - Musictown
- **Game Music**: "Cyber Race" - FASSounds
- **Original Concept**: Classic Snake Game

## Version History

**v1.0.1** (Current)
- SDL2 cross-platform support
- Improved scaling for modern displays
- Bug fixes and stability improvements

**v1.0.0** (2003)
- Initial Windows-only release
- FMOD audio support
- Classic and Remix game modes

