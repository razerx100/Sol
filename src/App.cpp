#include <App.hpp>
#include <string>

#include <BasicModels.hpp>
#include <Sol.hpp>
#include <DirectXColors.h>

App::App() {
	Sol::textureAtlas->SetTextureFormat(TextureFormat::UINT8);

	RGBA8 red = { 255u, 0u, 0u, 1u };

	Sol::textureAtlas->AddColour("Fuchsia", DirectX::Colors::Fuchsia);
	Sol::textureAtlas->AddColour("Cyan", DirectX::Colors::Cyan);
	Sol::textureAtlas->AddColour("Red", red);
	Sol::textureAtlas->AddColour("Green", DirectX::Colors::Green);
	Sol::textureAtlas->AddColour("Blue", DirectX::Colors::Blue);

	std::unique_ptr<Triangle> triangle0 = std::make_unique<Triangle>();
	std::unique_ptr<Triangle1> triangle1 = std::make_unique<Triangle1>();

	triangle0->SetTextureIndex(0u);
	triangle1->SetTextureIndex(0u);

	m_triangleRefs.emplace_back(
		Sol::modelContainer->AddModel(std::move(triangle0))
	);
	m_triangleRefs.emplace_back(
		Sol::modelContainer->AddModel(std::move(triangle1))
	);

	for(auto& triangle : m_triangleRefs)
		Sol::renderer->SubmitModel(triangle);
}

void App::SetResources() {
	auto fuchsia = Sol::textureAtlas->GetPixelData("Fuchsia");
	auto cyan = Sol::textureAtlas->GetPixelData("Cyan");
	std::uint32_t atlasWidth = Sol::textureAtlas->GetWidth();
	std::uint32_t atlasHeight = Sol::textureAtlas->GetHeight();

	m_triangleRefs[0]->SetTextureInfo(
		TextureData{
			fuchsia.uStart, fuchsia.uEnd, atlasWidth,
			fuchsia.vStart, fuchsia.vEnd, atlasHeight
		}
	);
	m_triangleRefs[1]->SetTextureInfo(
		TextureData{
			cyan.uStart, cyan.uEnd, atlasWidth,
			cyan.vStart, cyan.vEnd, atlasHeight
		}
	);
}

void App::Update() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::One)) {
		auto fuchsia = Sol::textureAtlas->GetPixelData("Fuchsia");
		std::uint32_t atlasWidth = Sol::textureAtlas->GetWidth();
		std::uint32_t atlasHeight = Sol::textureAtlas->GetHeight();

		m_triangleRefs[0]->SetTextureInfo(
			TextureData{
				fuchsia.uStart, fuchsia.uEnd, atlasWidth,
				fuchsia.vStart, fuchsia.vEnd, atlasHeight
			}
		);
	}
	else if(pKeyboardRef->AreKeysPressed(2, SKeyCodes::R, SKeyCodes::One)) {
		auto red = Sol::textureAtlas->GetPixelData("Red");
		std::uint32_t atlasWidth = Sol::textureAtlas->GetWidth();
		std::uint32_t atlasHeight = Sol::textureAtlas->GetHeight();

		m_triangleRefs[0]->SetTextureInfo(
			TextureData{
				red.uStart, red.uEnd, atlasWidth,
				red.vStart, red.vEnd, atlasHeight
			}
		);
	}

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		Sol::window->SetTitle("Alt + D");

	IMouse* pMouseRef = Sol::ioMan->GetMouseByIndex();

	if (pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Middle))
		Sol::window->SetTitle("Left + Right");

	IGamepad* pGamepadRef = Sol::ioMan->GetGamepadByIndex();

	if (auto e = pGamepadRef->Read();
		e.GetType() == IGamepad::Event::Type::LeftThumbStick)
		Sol::window->SetTitle(
			"M: " + std::to_string(e.GetMagnitude()) +
			" X: " + std::to_string(e.GetASData().xDirection) +
			" Y: " + std::to_string(e.GetASData().yDirection)
			);

	if (pGamepadRef->AreButtonsPressed(2, XBoxButton::START, XBoxButton::X))
		Sol::window->SetTitle("Start + A");
}
