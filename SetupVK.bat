if not exist Solution\ (
    mkdir .\Solution
)
cd .\Solution
cmake .. -G "Visual Studio 16 2019" -A x64 -DIMGUIFLAG:STRING=ENABLE -DINPUT_MANAGER:STRING=Pluto -DGRAPHICS_ENGINE:STRING=Terra -DRENDER_WINDOW:STRING=Luna