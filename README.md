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

Use the ***config.ini*** file from the executable directory to change the modules during the app launch. The file will be automatically generated with default values, if it doesn't exist.

Currently supported changeable modules/systems:\
<ins> Modules </ins> Renderer = Gaia  - DirectX12 based.\
<ins> Modules </ins> Renderer = Terra - Vulkan based.

<ins> Systems </ins> RenderEngine = IndirectDraw   - Compute Shader based Frustum Culling with Indirect Draw calls to Vertex shaders (AABB based).\
<ins> Systems </ins> RenderEngine = IndividualDraw - Legacy draw calls to Vertex shaders.\
<ins> Systems </ins> RenderEngine = MeshDraw       - Amplification/Task shader based Frustum (Sphere based) and Cluster culling with calls to Mesh shaders.

## Resources
### Default Window Icon
Default window icon location is {executable_directory}/resources/icon/Sol.ico.

## Executable icon
To change the executable icon for Windows, add the path to the icon in ***icon/win32ExecutableIcon.rc***.

## Requirements
cmake 3.21+.\
C++20 Standard supported Compiler.\
And requirements of the dependencies.
