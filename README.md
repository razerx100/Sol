# Sol
Manager module/executable which dynamically links to different modules to form a game engine. Modules must have required interfaces implemented.

# Dependencies
1. [Pluto](https://github.com/razerx100/Pluto).
2. [GaiaX](https://github.com/razerx100/GaiaX).
3. [Terra](https://github.com/razerx100/Terra).
4. [Luna](https://github.com/razerx100/Luna).
5. [Venus](https://github.com/razerx100/Venus).

## Library used
1. [Ceres](https://github.com/razerx100/Ceres).
2. [Europa](https://github.com/razerx100/Europa).

## Third party library used
1. [ImGui](https://github.com/ocornut/imgui).

## Instructions
Run the appropriate Setup script to configure the project with or without ImGui. The setup scripts use the ***Visual Studio 17 2022*** generator for project generation. But ***Visual Studio 16 2019*** generator should work as well.

## Resources
### Default Window Icon
Default window icon location is {executable_directory}/resources/icon/Sol.ico.

### Shader Location
Put the compiled shaders into {executable_directory}/resources/shaders/ directory.

## Executable icon
To change the executable icon for Windows, add the path to the icon in ***icon/win32ExecutableIcon.rc***. 

## Requirements
cmake 3.21+.\
C++20 Standard supported Compiler.\
And requirements of the dependencies.
