# ArtCore Engine
This is the main executable for ArtCore games. It is a game engine that can run games created in the ArtCore Editor. <br>
<hr>

## ArtCore
ArtCore is a 2D game engine, currently for Windows, later for Android and Linux.
The whole system consists of 3 components: engine, editor and compiler. For more information about the editor or compiler, go to their repositories. <br>

## Compilation
ArtCore Engine is created in Visual Studio 2022. There is no cmake file to create new projects. <br>
There is no external dependencies, project include all necessary files. The only exception are tests, for which there is a Google test API: <br>
To install using vcpkg type:<br>
> vcpkg gtest:x64-windows 
<br>
This is included as source files:<br>
SDL2 (input and system), SDL2_GPU (graphic render), PHYSFS (game archive), JSON (Nlohmann version), and PLF (for data structures and dynamic containers)

## Game archive
Game engine needs game data archives to run. Program loads them from main directory or gets path from starting arguments (all arguments are listed later)<br>
* game.dat <- contains main game scripts, object definitions and scene definitions. Also contains basic core files like the default font, script library and shaders.
* assets.pak <- contains all game assets: sprites, textures, music, sounds, and fonts.
* Platform.dat <- The first loaded file contains the configuration file and user saves and data. In setup.ini, there are SDL or openGL starting arguments. <br>
The engine uses physfs library to read data from archives. All assets are loaded when the game starts.

## Configuration
There are three configurations: debug, debug editor, and release.
* Debug - is for debugging in Visual Studio. This configuration loads game files from the "test" directory, opens a console window and outputs every game message. Has codebreak asserts on errors.
* DebugEditor <- is for editor debug mode. Output is redirected to the editor console.
* Release <- is for shipping. Warnings and small errors are ignored.
All configurations catch any thrown error in the main and dumb console output to the file "current_data_and_time.log."

## Technology
The project is targeted for x64 platforms. <br>
The engine is written in C++ and uses SDL2, SDL2_gpu, PhysFS, and JSON. <br>
The code is compatible with C++20. Some of used libraries are writen in C, but most of them have custom c++ wrappers.<br>
SDL2 and SDL subsystems are dynamically linked and use DLLs. Most assets data structures are written in JSON. <br>
There are two files in the header that are compiled to executables: console font and splash screen (the ArtCore logo). <br>
The engine is designed to run only prepared script files, not as a standalone program. All components are written without
relying on one platform (in the future porting to other platforms).<br>

## Scripts
### Core.tar
Core.tar is a file that contains all core files for the engine. It is used to create a new game archive. <br>
To make archive, run "create_core_tar.bat" in the main directory and open it in ArtCore Editor (Project->Update core)<br>

### ArtLibGenerator.exe
ArtLibGenerator is a program that generates a library of functions for the ArtScript language. <br>
It searchs function comment, then creates function declaration in "CodeExecutor.h" and "CodeExecutor_functions_map.cpp"<br>
This is the way to add new functions to the ArtScript language.

# Game Engine
<hr>

## Debug mode and game console
The engine has a runtime console, to run press HOME button on keyboard. Console contains all game logs and warnings and can execute simple commands (more later).<br>
To enable debug mode, compile with AC_ENABLE_DEBUG_MODE in main.h and press F1 in the game. The debug menu contains information about spawned instances and measures performance.
* Performance (F2) <- Show memory info. There is a global stack size, must be always 0, otherwise game has memory leak. More about global stacking later.
* Instance info (F3) <- Show all instances name#id and world position. ID is important for the spy window.
* Collider`s (F4) <- Draw a blue frame of instances collision mask (body).
* Direction`s (F5) <- Draw instances directions as a yellow-green line.
* Spy window (F6) <- Show all instance data, variables and other info. To activate must type in the game console "spy [id]". For check instance ID, I would press F3.
* Performance counters (F7) <- They show execution real time (each frame) of individual steps in a game loop.
## Engine workflow
Every frame has seven steps:
1. ProcessEvents <- Process all user input and system events, like mouse, keyboard or game controller.
2. ProcessStep <- Main logic update for every instance; first spawn all new instances, later check for events and execute them if they occur.
3. ProcessPhysics <- Check all collisions and move instances if needed.
4. ProcessSceneRender <- Render all instances on scene.
5. ProcessPostProcessRender <- Render game interface (gui system) and apply post process.
6. ProcessSystemRender <- Render console and debug info.
7. Flip everything on the screen.

## Assets
There are 5 asset types:
1. Texture <- Simple png file.
2. Sprite <- Collection of textures that can be animated. Contains mask used by On Click event.
3. Music <- A music file that can be played in the background.
4. Sound <- A short audio file that can be played in a specific location (3D audio).
5. Font <- The font used to draw on screen.

## Objects (Instances) aka Entity
Objects are defined in the editor and may have a body, sprite, script, and other properties. They are spawned in the scene and have unique ID's. <br>
Each frame the engine updates all instances, checks events and executes them if occur. <br>
Object is definition of instance, instance is spawned object. Instances can be referenced by name, tag, or id. Each ID is unique, but tags
can be the same for many instances. The scripts used by instances (and scenes) are written in ArtScript, a custom scripting language.

## Scene
Scene is a container for instances. It is defined in the editor. Scenes can have a background color or background texture. <br>
Scenes can have scripts. Each scene has levels that contain the starting positions of objects.
The engine needs at least one scene to run. <br>
Scene has GuiSystem (user interface) which is working regardless of instances. GuiSystem can be used to create buttons, text, images and other elements.
Each element on Gui has his own events and is always drawn on top of a screen without post process efect.<br>
Scene can be used for storing global variables that have not been erased between levels.

## Event system
Each instance can have programmed events; if they occur and meet the conditions, they are executed in hierarchies:
1. On create <- on spawn instance to scene
2. On destroy <- when instance is deleted
3. Step <- logic update on every frame
4. On view enter/leave <- when instance is outside screen or enters screen
5. Mouse event (pressed, released) on global mouse events. (on Click) if sprite mask has been pressed.
6. Keyboard event "on key pressed or released"

7. Controller <- button or joystick (not implemented yet) (in version 1.0)
8. Toutch screen (as mouse event) (not implemented yet) (in version 1.0)

## Body, mask, collisions
Every instance can have a body: rectangle or circle. There is an option to copy the body from the sprite mask. <br>
The instance body and sprite mask work independently. Collision event trigger only for one instance, but they can reference to each other.
A collision event can be executed only if both instances have bodies. <br>

## ArtScript
ArtScript is a custom scripting language, created for ArtCore Engine. It is based on Pascal but has many differences.
For now, it contains only basic functions and operators. There are no dynamic variables or loops. Script is compiled to bytecode and executed by a virtual machine.
Scripts can be executed by instance or scene. For more info abut ArtScript or Compiler see ArtCompiler repo. Documentation of all script functions is in the ArtScript.md file.
