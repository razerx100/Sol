#include <Window.hpp>
#include <PipelineManager.hpp>
#include <App.hpp>
#include <Keyboard.hpp>
#include <Mouse.hpp>

std::unique_ptr<Keyboard> GenericSingleton<Keyboard>::s_instance;
std::unique_ptr<Mouse> GenericSingleton<Mouse>::s_instance;

std::unique_ptr<Window> GenericSingleton<Window>::s_instance;
std::unique_ptr<PipelineManager> GenericSingleton<PipelineManager>::s_instance;
std::unique_ptr<App> GenericSingleton<App>::s_instance;

