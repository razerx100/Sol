#include <App.hpp>
#include <string>

#include <BasicModels.hpp>
#include <Sol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>

App::App() {
	Sol::textureAtlas->SetTextureFormat(TextureFormat::UINT8);

	RGBA8 red = { 255u, 0u, 0u, 1u };

	Sol::textureAtlas->AddColour("Fuchsia", DirectX::Colors::Fuchsia);
	Sol::textureAtlas->AddColour("Cyan", DirectX::Colors::Cyan);
	Sol::textureAtlas->AddColour("Red", red);
	Sol::textureAtlas->AddColour("Green", DirectX::Colors::Green);
	Sol::textureAtlas->AddColour("Blue", DirectX::Colors::Blue);

	std::unique_ptr<Cube> cube0 = std::make_unique<Cube>();
	std::unique_ptr<Cube> cube1 = std::make_unique<Cube>();

	cube0->SetTextureIndex(0u);
	cube1->SetTextureIndex(0u);

	DirectX::XMVECTOR rotAxis = { 0.f, 1.f, 0.f, 0.f };

	cube1->AddTransformation(
		DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(45))
		* DirectX::XMMatrixTranslation(-0.1f, 0.f, 0.3f)
	);

	cube0->AddTransformation(
		DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(45))
	);

	m_modelRefs.emplace_back(
		Sol::modelContainer->AddModel(std::move(cube0))
	);
	m_modelRefs.emplace_back(
		Sol::modelContainer->AddModel(std::move(cube1))
	);

	for(auto& triangle : m_modelRefs)
		Sol::renderer->SubmitModel(triangle);

	// Add two cameras
	DirectX::XMFLOAT3 cameraPosition = { 0.f, 0.f, -1.f };
	DirectX::XMFLOAT3 focusPosition = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 upVector = { 0.f, 1.f, 0.f };

	PerspectiveCamera camera1;
	camera1.SetFocusPosition(focusPosition);
	camera1.SetCameraPosition(cameraPosition);

	size_t camera1Index = Sol::cameraManager->AddPerspectiveCameraAndGetIndex(camera1);

	cameraPosition.z = 1.f;

	PerspectiveCamera camera2;
	camera2.SetFocusPosition(focusPosition);
	camera2.SetCameraPosition(cameraPosition);

	size_t camera2Index = Sol::cameraManager->AddPerspectiveCameraAndGetIndex(camera2);

	Sol::renderer->SetViewMatrix(
		Sol::cameraManager->GetPerspectiveCameraByIndex(camera1Index).GetViewMatrix()
	);
}

void App::SetResources() {
	auto fuchsia = Sol::textureAtlas->GetPixelData("Fuchsia");
	auto cyan = Sol::textureAtlas->GetPixelData("Cyan");
	std::uint32_t atlasWidth = Sol::textureAtlas->GetWidth();
	std::uint32_t atlasHeight = Sol::textureAtlas->GetHeight();

	m_modelRefs[0]->SetTextureInfo(
		TextureData{
			fuchsia.uStart, fuchsia.uEnd, atlasWidth,
			fuchsia.vStart, fuchsia.vEnd, atlasHeight
		}
	);
	m_modelRefs[1]->SetTextureInfo(
		TextureData{
			cyan.uStart, cyan.uEnd, atlasWidth,
			cyan.vStart, cyan.vEnd, atlasHeight
		}
	);
}

void App::PerFrameUpdate() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::One)) {
		auto fuchsia = Sol::textureAtlas->GetPixelData("Fuchsia");
		std::uint32_t atlasWidth = Sol::textureAtlas->GetWidth();
		std::uint32_t atlasHeight = Sol::textureAtlas->GetHeight();

		m_modelRefs[0]->SetTextureInfo(
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

		m_modelRefs[0]->SetTextureInfo(
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

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::UpArrow))
		Sol::renderer->SetViewMatrix(
			Sol::cameraManager->GetPerspectiveCameraByIndex(0u).GetViewMatrix()
		);

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::DownArrow))
		Sol::renderer->SetViewMatrix(
			Sol::cameraManager->GetPerspectiveCameraByIndex(1u).GetViewMatrix()
		);
}

void App::PhysicsUpdate() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::W, SKeyCodes::Two))
		m_modelRefs[1]->AddTransformation(
			DirectX::XMMatrixTranslation(-0.1f, 0.f, 0.3f)
		);

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::S, SKeyCodes::Two))
		m_modelRefs[1]->AddTransformation(
			DirectX::XMMatrixTranslation(0.1f, 0.f, -0.3f)
		);
}
