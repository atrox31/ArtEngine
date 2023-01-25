# ArtCore Engine
This is the main executable for ArtCore games. It is a game engine that can run games created in the ArtCore Editor. <br>
<hr>
ArtCore is a 2D game engine, currently for Windows, later for Android and Linux.
The whole system consists of 3 components: engine, editor and compilator. For more information about the editor or compiler, go to their repositories. <br>

## Compilation
ArtCore Engine is created in Visual Studio 2022. There is no cmake file to create new projects. <br>
There is no aditional depedences, project include all nessesary files. The only exception are tests, for which there is a Google test API: <br>
To install using vcpkg type:<br>
> vcpkg gtest:x64-windows <br>
This is included as source files:<br>
SDL2 (input and system), SDL2_GPU (graphic render), PHYSFS (game archive), JSON (Nlohmann version), and PLF (for data structures and dynamic containers)

## Game archive
Game engines need game data archives to run. Program loads it from main directory or gets path from starting arguments (all arguments are listed later)<br>
* game.dat    <- contains main game scripts, object definitions, and scene definitions. also contains basic core files like the default font, script library, and shaders.
* assets.pak    <- contains all game assets: sprites, textures, music, sounds, and fonts.
* Platform.dat - The first loaded file contains the configuration file and user saves and data. In setup.ini, there are sdl or openGL starting arguments. <br>
engine using physfs archives to mount and read data from archives. All assets are loaded when the game starts.

## Configurations
There are three configurations: debug, debug editor, and release.
* Debug - is for debugging in Visual Studio. This configuration loads game files from the "test" directory. opens a console window and outputs every game message. Have codebrak asserts en errors.
* DebugEditor <- is for editor debug mode. Output is redirected to the editor console. On errors try to get message and close itself.
* Release <- Is for shipping. On warrings and small errors try to work anyway.
All configurations catch any thrown error in the main and dumb console output to the file "current_data_and_time.log."

## Technology
The project is targeted for x64 platforms. <br>
The engine is written in C++ and uses SDL2, SDL2_gpu, PhysFS, and JSON. <br>
The code is compatible with C++20. Some of used librays are writen in C but most of it have custom c++ wrappers.<br>
SDL2 and SDL subsystems are dynamically linked and use DLLs. Most asset data structures are written in JSON. <br>
There are two files in the header that are compiled to executables: console font and splash screen (the ArtCore logo). <br>
Engine is designed to run only prepared script files, not as standalone program. All components are written without
depetences only on one platform for future porting to other platforms.<br>

# Game Engine
<hr>

## Debug mode and game console
Engine have runtime console, to run press HOME button on keyboard. Console contains all game logs and warrings and can execute simple commands (more later).<br>
To enable debug mode, compile with AC_ENABLE_DEBUG_MODE in main.h and press F1 in the game. The debug menu contains information about spawned instances and measures performance.
* Performance (F2)  Show memory info. There is global stack size, must be always 0, otherwise game have memory leak. More about global stacking later.
* Instance info (F3) Show all instance name#id and world position. ID is important for the spy window.
* Collider`s (F4) <- Draw blue frame of instances collision mask (body).
* Direction`s (F5) <- Draw instances' directions as a yellow-green line.
* Spy window (F6) <- Show all instance data, variables, and other info. To activate must type in game console "spy [id]". For instance, I would press F3.
* Performance counters (F7): Show time in millisecconds about game indywidual game update loop phases.

Engine workflow
Every frame has seven phases.
1. ProcessEvents <- Process all user input and system events, like mouse keyboard or game controller.
2. ProcessStep: Main logic update for every instance; first spawn all new instances, later check for events, and execute them if they occur.
3. ProcessPhysics: Check all collisions and move instances if needed.
4. ProcessSceneRender <- Render all instances on scene.
5. ProcessPostProcessRender <- Render game interface (gui system) and apply post process.
6. ProcessSystemRender: Render console and debug info
7. Flip everything on the screen.

## Assets
There are 5 asset types:
1. Texture
2. Sprite
3. Music
4. Sound
5. Font <- 

## Objects (Instances) aka Entity
Objects are defined in the editor and may have a body, sprite, script, and other properties. They are spawned in the scene and have a unique ID. <br>
Each frame engine update all instances, check events and execute them if occurs. They can be destroyed by a script or by an engine. <br>
Object is definition of instance, instance is spawned object. Instances can be referenced by name, tag, or id. Each ID is unique, but tags
can be same for many instances. The scripts used by instances (and scenes) are written in ArtScript, a custom scripting language.

## Scene
Scene is a container for instances. They are defined in the editor and can be loaded by script. Scenes can have a background color or background texture. <br>
Scenes can have scripts, which are executed on scene load. Each scene has levels that contain the starting positions of objects.
Engines need at least one scene to run. <br>
Scene have GuiSystem (user interface) witch working independed of instances. GuiSystem can be used to create buttons, text, images, and other elements.
Each element on gui have own events and always draw on top of screen witchout post process efect.<br>
Scene can be used for storing global variables that have not been erased between levels.

## Event system
Each instance can have programmed events; if they occur and meet the conditions, they are executed in hierarchies:
1. On create <- on spawn instance to scene
2. On destroy <- when instance is deleted
3. Step <- logic update on every frame
4. On view enter/leave <- when instance is outside screen or enters screen
5. Mouse event (pressed, released, mask pressed)  on global mouse events. Every sprite can have mask, this mean if sprite has clicked.
6. Keyboard event "on key pressed or released"
(in version 1.0)
7. Controller <- button or joystick (not implemented yet)
8. Toutch screen (as mouse event) (not implemented yet)

## Body, mask, collisions
Every instance can have a body, rectangle, or circle. They are defined in the editor or via script. There is an option to copy the body from the sprite mask. <br>
The instance body and sprite mask work independently. Collision event rise only for one instance, but they can reference to eachother.
A collision event can be executed only if both instances have bodies. If one of them is missing, the collision event will not rise. <br>

## ArtScript
ArtScript is custom scripting language, created for ArtCore Engine. It is based on Lua or Pascal but has many differences.
For now, it contains only basic functions and operators. There are no dynamic variables or loops. Script is compiled to bytecode and executed by a virtual machine.
Scripts can be executed by instance or scene. For more info abut ArtScript or Compilator see ArtCompiler repo. Documentation of all script functions is in the ArtScript.md file.