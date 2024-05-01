# Sol
Manager module/executable which dynamically links to different modules to form a game engine. Modules must have the required interfaces implemented.

# Dependencies
1. [Pluto](https://github.com/razerx100/Pluto).
2. [GaiaX](https://github.com/razerx100/GaiaX).
3. [Terra](https://github.com/razerx100/Terra).
4. [Luna](https://github.com/razerx100/Luna).
5. [Venus](https://github.com/razerx100/Venus).
6. [Europa](https://github.com/razerx100/Europa).

## Third party library used
1. [DirectXMath](https://github.com/microsoft/DirectXMath).
2. [stb](https://github.com/nothings/stb).

## Instructions
Use the ADD_TEST_SOL cmake flag to add unit testing.

Use the ***config.ini*** file from the executable directory to change the modules at the start of the app. The file will be automatically generated, if it doesn't exist upon starting the app.

Currently supported changeable modules/systems:\
[Modules] Renderer = Gaia\
[Modules] Renderer = Terra

[Systems] RenderEngine = IndirectDraw\
[Systems] RenderEngine = IndividualDraw

## Resources
### Default Window Icon
Default window icon location is {executable_directory}/resources/icon/Sol.ico.

## Executable icon
To change the executable icon for Windows, add the path to the icon in ***icon/win32ExecutableIcon.rc***. 

## Requirements
cmake 3.21+.\
C++20 Standard supported Compiler.\
And requirements of the dependencies.
