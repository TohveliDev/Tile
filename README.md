![Logo](/docs/logo.png)

License: Tile is availaible under 2 licenses, The Happy Bunny License and the Beerware License. For more information, see [LICENSE](/docs/LICENSE.txt).

## About Tile

Tile (**T**ile **i**s a **L**evel **E**ditor) is a WebGL 2.0 and Emscripten based tilemap editor, that is specifically made for OpenGL / SFML Based C++ Projects, with additional JSON support for other purposes.

The main difference between Tile and various other map editors, is that Tile is ran in a browser, so no external executables are needed. This also makes it multiplatform, as the only requirement you need for your PC to run Tile, is to have a web browser.

Tile can be used to create maps of various styles and sizes. You can create a 1000x1000 tile map with only one layer, or a 10x10 map with 100 layers. The only limiting thing is your imagination!

Tile's own tilemap format (.tile) provides an easy way to quickly jump back into your project, no matter where you are, as every every texture you have imported is automatically Base64 encoded and saved into the file, which is then converted back into a usable texture after you load back your previous project.

## Tilemap Formats

Tile currently can export your project into 3 different formats:

1. Text File (.txt), where the content can just be Copy & Pasted to your C++ Project
2. C++ Header File (.h), where you can start using the map by including the header in your main.cpp
3. JSON, which is compatible with projects created with other tilemap editors.

## Compiling Tile

To compile Tile on your own, you just need Docker. The repository has all the files you need for a simple and quick compiling via your preferred terminal.

1. ``docker compose up``
2. ``docker compose build``