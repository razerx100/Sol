# Sol
Game Engine which can statically link to different 3D engines. (Currently can only link to Gaia. But hopefully more will be supported in the future)

## Third party library Used
ImGui

## Instructions
Put desired engine module's static lib in ***libs*** directory, then run the appropriate Setup script to build the project with or without common ImGui files.

#### Executable icon
To change the executable icon for Windows, add the path to the icon in ***icon/win32ExecutableIcon.rc***.

## Requirements
cmake 3.11+.\
C++20 Standard supported Compiler.
