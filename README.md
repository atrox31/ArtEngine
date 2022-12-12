# How to run this
Download release or clone and compile code. Move "assets.pak" and "game.dat" from any project to the folder that contains ArtCore.exe and run the executable.</br>

# ArtCore
What is ArtCore? 2D game engine that can draw textures and animate sprites, play music, sound, render geometry and text. Game objects can collide with each other and react to global events like mouse input or keyboard. Base code is created in the game editor in ArtScript (C/pascal like script language created only for the ArtCore game engine). Every instance variable on executing script is stored in Stack (FILO first in last out). Every variable type has its own stack. Technology behind the scenes is SDL2 with SDL_GPU to maximize performance and use graphics card. </br>
For more information on how to create games in ArtCore engine see ArtEditor.</br>
## Commands
### In debug mode
F1 - Show info about instances: name, coords, center, collision mask and sprite mask.</br>
F2 - Show info about instances count, delta time (time to generate frame) and global stack size.
### Normal commands
F4 - Show fps</br>
[from 0.6.2]</br>
F5 - Enable/disable global bloom (shader)</br>
F6 - Set global bloom to low</br>
F7 - Set global bloom to medium</br>
F8 - Set global bloom to high</br>
