#include <App.hpp>
#include <string>
#include <TextureLoader.hpp>

#include <BasicModels.hpp>
#include <Sol.hpp>
#include <AppModules.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>

App::App() {
	RGBA8 red = { 255u, 0u, 0u, 1u };

	ColourTexture& colourManager = Sol::textureAtlas->GetColourTextureManager();

	colourManager.AddColour("Fuchsia", DirectX::Colors::Fuchsia);
	colourManager.AddColour("Cyan", DirectX::Colors::Cyan);
	colourManager.AddColour("Red", red);
	colourManager.AddColour("Green", DirectX::Colors::Green);
	colourManager.AddColour("Blue", DirectX::Colors::Blue);

	STexture segsTex = TextureLoader::LoadTextureFromFile("resources/textures/segs.jpg");

	Sol::textureAtlas->AddTexture(
		"segs", std::move(segsTex.data), segsTex.width, segsTex.height
	);

	STexture dogeTex = TextureLoader::LoadTextureFromFile("resources/textures/doge1.jpg");

	Sol::textureAtlas->AddTexture(
		"doge", std::move(dogeTex.data), dogeTex.width, dogeTex.height
	);

	STexture dogeBigTex = TextureLoader::LoadTextureFromFile("resources/textures/doge.jpg");

	Sol::textureAtlas->AddTexture(
		"dogeBig", std::move(dogeBigTex.data), dogeBigTex.width, dogeBigTex.height
	);

	std::shared_ptr<Cube> cube0 = std::make_unique<Cube>();
	std::shared_ptr<Cube> cube1 = std::make_unique<Cube>();
	std::shared_ptr<Quad> quad = std::make_unique<Quad>();

	cube0->SetTextureIndex(0u);
	cube1->SetTextureIndex(0u);
	quad->SetTextureIndex(0u);

	DirectX::XMVECTOR rotAxis = { 0.f, 1.f, 0.f, 0.f };

	cube1->AddTransformation(
		DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(45))
		* DirectX::XMMatrixTranslation(-0.1f, 0.f, 0.3f)
	);

	cube0->AddTransformation(
		DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(45))
	);

	quad->AddTransformation(
		DirectX::XMMatrixTranslation(0.5, 0.f, 0.f)
	);

	AddModel(std::move(cube0));
	AddModel(std::move(cube1));
	AddModel(std::move(quad));

	// Add two cameras
	DirectX::XMFLOAT3 cameraPosition = { 0.f, 0.f, -1.f };

	PerspectiveCameraEuler camera1;
	camera1.SetWorldForwardDirection(true);
	camera1.SetCameraPosition(cameraPosition);

	size_t camera1Index = AMods::cameraManager->AddEulerCameraAndGetIndex(
		std::make_unique<PerspectiveCameraEuler>(camera1)
	);

	cameraPosition.z = 1.f;

	PerspectiveCameraEuler camera2;
	camera2.SetWorldForwardDirection(false);
	camera2.SetCameraPosition(cameraPosition);

	size_t camera2Index = AMods::cameraManager->AddEulerCameraAndGetIndex(
		std::make_unique<PerspectiveCameraEuler>(camera2)
	);

	AMods::cameraManager->SetCurrentCameraIndex(camera1Index);
}

void App::SetResources() {
	auto fuchsia = Sol::textureAtlas->GetUVInfo("Fuchsia");
	auto cyan = Sol::textureAtlas->GetUVInfo("Cyan");
	auto doge = Sol::textureAtlas->GetUVInfo("doge");

	m_models[0].lock()->SetUVInfo(
		fuchsia.uOffset, fuchsia.vOffset, fuchsia.uRatio, fuchsia.vRatio
	);
	m_models[1].lock()->SetUVInfo(cyan.uOffset, cyan.vOffset, cyan.uRatio, cyan.vRatio);
	m_models[2].lock()->SetUVInfo(doge.uOffset, doge.vOffset, doge.uRatio, doge.vRatio);
}

void App::PerFrameUpdate() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboard();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::One)) {
		auto doge = Sol::textureAtlas->GetUVInfo("dogeBig");

		m_models[2].lock()->SetUVInfo(doge.uOffset, doge.vOffset, doge.uRatio, doge.vRatio);
	}
	else if(pKeyboardRef->AreKeysPressed(2, SKeyCodes::R, SKeyCodes::One)) {
		auto segs = Sol::textureAtlas->GetUVInfo("segs");

		m_models[2].lock()->SetUVInfo(segs.uOffset, segs.vOffset, segs.uRatio, segs.vRatio);
	}

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		Sol::window->SetTitle("Alt + D");

	IMouse* pMouseRef = Sol::ioMan->GetMouse();

	if (pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Middle))
		Sol::window->SetTitle("Left + Right");

	IGamepad* pGamepadRef = Sol::ioMan->GetGamepad();

	if (pGamepadRef->AreButtonsPressed(2, XBoxButton::START, XBoxButton::X))
		Sol::window->SetTitle("Start + A");

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::UpArrow))
		AMods::cameraManager->SetCurrentCameraIndex(0u);

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::DownArrow))
		AMods::cameraManager->SetCurrentCameraIndex(1u);

	AMods::cameraManager->SetCamera();
}

void App::PhysicsUpdate() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboard();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::W, SKeyCodes::Two))
		m_models[1].lock()->AddTransformation(
			DirectX::XMMatrixTranslation(-0.1f, 0.f, 0.3f)
		);

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::S, SKeyCodes::Two))
		m_models[1].lock()->AddTransformation(
			DirectX::XMMatrixTranslation(0.1f, 0.f, -0.3f)
		);

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::W)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveForward(AMods::frameTime->GetDeltaTime());
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::S)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveBackward(AMods::frameTime->GetDeltaTime());
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::A)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveLeft(AMods::frameTime->GetDeltaTime());
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::D)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveRight(AMods::frameTime->GetDeltaTime());
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::T)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveUp(AMods::frameTime->GetDeltaTime());
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::G)) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		camera->MoveDown(AMods::frameTime->GetDeltaTime());
	}

	IGamepad* pGamepadRef = Sol::ioMan->GetGamepad();

	if (auto thumbStickData = pGamepadRef->ReadRightThumbStickData(); thumbStickData) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		auto& [magnitude, offsetX, offsetY] = *thumbStickData;
		float deltaTime = AMods::frameTime->GetDeltaTime();

		camera->LookAround(offsetX * deltaTime * 3.f , -1.f * offsetY * deltaTime * 3.f);
	}
}

void App::AddModel(std::shared_ptr<Model> model) noexcept {
	Sol::modelContainer->AddModel(model);

	m_models.emplace_back(std::move(model));
}
