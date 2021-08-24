#include <Window.hpp>
#include <PipelineManager.hpp>
#include <App.hpp>
#include <Keyboard.hpp>
#include <Mouse.hpp>

std::unique_ptr<Keyboard> Keyboard::s_instance;
std::unique_ptr<Mouse> Mouse::s_instance;

std::unique_ptr<Window> Window::s_instance;
std::unique_ptr<PipelineManager> PipelineManager::s_instance;
std::unique_ptr<App> App::s_instance;

