# Sol
Manager module/executable which dynamically links to different modules to form a game engine. Modules must have required interfaces implemented.

# Dependencies
1. [Pluto](https://github.com/razerx100/Pluto).
2. [GaiaX](https://github.com/razerx100/GaiaX) or [Terra](https://github.com/razerx100/Terra).
3. [Luna](https://github.com/razerx100/Luna).

## Third party library Used
ImGui

## Instructions
Run the appropriate Setup script to configure the project with or without ImGui. Use the ones with VK suffix for using Vulkan renderer or Dx12 suffix for Directx12 renderer. Otherwise, either renderer's dll can be built seperately and put alongside with the executable. One still needs to be chosen for the import library's static linking (or import library binary maybe be provide instead to link with).

#### Executable icon
To change the executable icon for Windows, add the path to the icon in ***icon/win32ExecutableIcon.rc***.

## Requirements
cmake 3.21+.\
C++20 Standard supported Compiler.\
And requirements of the dependencies.
