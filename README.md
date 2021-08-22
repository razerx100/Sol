# Sol
Game Engine which can statically link to different 3D engines. (Currently can only link to Gaia. But hopefully more will be supported in the future)

## Third party library Used
ImGui

## Instructions
Put desired engine module's static lib in ***libs*** directory, then run the appropriate Setup script to build the project with or without common ImGui files.

### Window icon
#### Win32
To change the window icon, put any icon with .ico extension in ***icon*** directory and name it Sol.ico. Or change the default window icon name in ***src/resource.rc*** to match your icon name.

## Requirements
cmake 3.11+.\
C++20 Standard supported Compiler.
