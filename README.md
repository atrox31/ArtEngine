# ArtCore Engine
This is main executable for ArtCore games. 
<hr>

## ArtCore
ArtCore is 2D game engine, currently for Windows, later Android and Linux. The whole system consists of 3 components: engine, editor and compilator. For more information about editor or compilator go to their repositiries.<br>

## Compilation
ArtCore Engine is created in Visual Studio 2022. There is no cmake file to create new projects.<br>
There is no aditional depedences, project include all nessesary files. Only exception are tests, there are using Google test api:<br>
To install using vcpkg type:<br>
> vcpkg gtest:x64-windows <br>
This is included as source files:<br>
SDL2(input and system), SDL2_GPU(graphic render), PHYSFS(game archive), JSON(nlohmann version), PLF(for data structures and dynamic containers)

## Game archive
Game engine to run need game data archives. Program loads it from main directory or gets path from starting arguments (all arguments are listed later)<br>
* game.dat	<- Contains main game scripts, object definitions and scenes definitions. Also contains basic core files like default font, script libray and shaders.
* assets.pak	<- Contains all game assets: Sprites, Textures, Musics, Sounds, Fonts.
* Platform.dat	<- First loaded file, contains configuration file and user saves/data. In setup.ini there is sdl or opengl starting arguments.<br>
Engine using physfs archives to mount and read data from archives. All assets are loaded when game starts.

## Configurations
There are 3 configurations: Debug, DebugEditor and Release.<br>
* Debug <- Is for debuging in visual studio. This configuration loads game files from "test" directory. Opens with console window and output every game message. Have codebrak asserts en errors.
* DebugEditor <- Is for editor debug mode. Output is redirected to editor console. On errors try to get message and close itself.
* Release <- Is for shipping. On warrings and small errors try to work anyway.
All configurations catch any throwed error in main and dumb console output to file "current_data_and_time.log"

## Technology
Project is targeted for x64 platforms.<br>
Code is compatybile with C++20. Some of used librays are writen in C but most of it have custom c++ wrappers.<br>
SDL2 and SDL subsystems are dynamic linking and use DLL`s. Most of asset data structure are written in json.<br>
There are two files in header that are compiled to executable, console font and spash screen (ArtCore logo).<br>
Engine is designed to run only prepared script files, not as standalone program.

# Game Engine
<hr>

## Debug mode and game console
Engine have runtime console, to run press HOME button on keyboard. Console contains all game logs and warrings and can execute simple commands (more later).<br>
To enable debug mode compile with AC_ENABLE_DEBUG_MODE in main.h and press F1 in game. Debug menu contains informations about spawned instances and measure preformance.
* Performance (F2) <- Show memory info. There is global stack size, must be always 0, otherwise game have memory leak. More about global stack later.
* Instance info (F3) <- Show all instance name#id and world position. Id is important for spy window.
* Collider`s (F4) <- Draw blue frame of instances collision mask (body).
* Direction`s (F5) <- Draw instances direction as yellow-green line.
* Spy window (F6) <- Show all instance data, varaibles and other info. To activate must type in game console "spy [id]". For instance id press F3.
* Performance counters (F7) <- Show time in millisecconds about game indywidual game update loop phases.

## Engine workflow
Every frame have 7 phases.
1. ProcessEvents <- Process all user input and system events, like mouse keyboard or game controller.
2. ProcessStep <- Main logic update for every instance, first spawn all new instances, later check events and execute it if occurs.
3. ProcessPhysics <- Check all collisions and move instances if need to.
4. ProcessSceneRender <- Render all instances on scene.
5. ProcessPostProcessRender <- Render game interface (gui system) and apply post process.
6. ProcessSystemRender <- Render console and debug info
7. Flip everything on screen.

## Assets
There are 5 assets type:
1. Texture <- 
2. Sprite <- 
3. Music <- 
4. Sound <- 
5. Font <- 

## Objects (Instances) aka Entity
Objects are defined in editor, may have body, sprite, script and other properties. They are spawned in scene and have unique id.<br>
Each frame engine update all instances, check events and execute them if occurs. They can be destroyed by script or by engine.<br>
Object is definition of instance, instance is spawned object. Instances can be referenced by name, tag or id. Each id is unique, but tag
can be same for many instances. Scripts used by instances (and scene) are written in ArtScript, custom script language.

## Scene
Scene is a container for instances. They are defined in editor and can be loaded by script. Scene can have background color or background texture.<br>
Scene can have script, which is executed on scene load. Each scene have levels, that contains starting position of objects.
Engine need at leat one scene to run.<br>
Scene have GuiSystem (user interface) witch working independed of instances. GuiSystem can be used to create buttons, text, images and other elements.
Each element on gui have own events and always draw on top of screen witchout post process efect.<br>
Scene can be used for storing global varaibles that not been erased beetwen level.

## Event system
Each instance can have programmed events, if they occur and meet the conditions, they are executed in hierarhy:
1. On create <- on spawn instance to scene
2. On destroy <- when instance is deleted
3. Step <- logic update on every frame
4. On view enter/leave <- when instance is outside screen or enters screen
5. Mouse event (pressed, released, mask pressed) <- On global mouse events. Every sprite can have mask, this mean if sprite has clicked.
6. Keyboard event <- on key pressed or released
(in version 1.0)
7. Controller <- button or joystick (not implemented yet)
8. Toutch screen (as mouse event) (not implemented yet)

## Body, mask, collisions
Every instance can have body, rectangle or circle. They are defined in Editor or via script. There is an option to copy body from sprite mask.<br>
Instance body and sprite mask work independed. Collision event rise only for one instance, but they can reference to eachother.
Collision event can be executed only if both instances have body. If one of them is missing, collision event will not rise.<br>

## ArtScript
ArtScript is custom scripting language, created for ArtCore Engine. It is based on Lua/Pascal, but have many differences. 
For now contains only basic functions and operators. There is no dynamic variables or loops. Script is compiled to bytecode and executed by virtual machine.
Script can be executed by instance or scene. For more info abut ArtScript or Compilator see ArtCompiler repo. Documentation of all scripts functions are in ArtScript.md file.

