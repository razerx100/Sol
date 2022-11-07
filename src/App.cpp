#include <App.hpp>
#include <string>
#include <TextureLoader.hpp>

#include <BasicModels.hpp>
#include <Sol.hpp>
#include <AppModules.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>

App::App() {
	RGBA8 red{ 255u, 0u, 0u, 255u };

	Sol::textureAtlas->GetColourTextureManager()
		.AddColour("Fuchsia", DirectX::Colors::Fuchsia)
		.AddColour("Cyan", DirectX::Colors::Cyan)
		.AddColour("Red", red)
		.AddColour("Green", DirectX::Colors::Green)
		.AddColour("Blue", DirectX::Colors::Blue);

	TextureLoader::AddTextureToAtlas("resources/textures/segs.jpg", "segs");
	TextureLoader::AddTextureToAtlas("resources/textures/doge1.jpg", "doge");
	TextureLoader::AddTextureToAtlas("resources/textures/doge.jpg", "dogeBig");

	auto cubes = AddAndGetModels<Cube, CubeInputs>(2u);
	auto quads = AddAndGetModels<Quad, QuadInputs>(1u);

	AddModels(std::move(cubes)).AddModels(std::move(quads));

	auto& cube0 = m_models[0];
	auto& cube1 = m_models[1];
	auto& quad = m_models[2];

	cube0->SetTextureIndex(0u);
	cube1->SetTextureIndex(0u);
	quad->SetTextureIndex(0u);

	cube1->GetTransform().RotateYDegree(45.f).MoveTowardsZ(-0.5f).MoveTowardsX(-1.5f);
	cube0->GetTransform().RotateYDegree(45.f);
	quad->GetTransform().MoveTowardsX(2.5f);

	// Add two cameras
	DirectX::XMFLOAT3 cameraPosition = { 0.f, 0.f, -1.f };

	PerspectiveCameraEuler camera1;

	camera1.SetWorldForwardDirection(true).SetCameraPosition(cameraPosition);

	size_t camera1Index = AMods::cameraManager->AddEulerCameraAndGetIndex(
		std::make_unique<PerspectiveCameraEuler>(camera1)
	);

	cameraPosition.z = 1.f;

	PerspectiveCameraEuler camera2;
	camera2.SetWorldForwardDirection(false).SetCameraPosition(cameraPosition);

	size_t camera2Index = AMods::cameraManager->AddEulerCameraAndGetIndex(
		std::make_unique<PerspectiveCameraEuler>(camera2)
	);

	AMods::cameraManager->SetCurrentCameraIndex(camera1Index);
}

void App::SetResources() {
	auto fuchsia = Sol::textureAtlas->GetUVInfo("Fuchsia");
	auto cyan = Sol::textureAtlas->GetUVInfo("Cyan");
	auto doge = Sol::textureAtlas->GetUVInfo("doge");

	m_models[0]->SetUVInfo(
		fuchsia.uOffset, fuchsia.vOffset, fuchsia.uRatio, fuchsia.vRatio
	);
	m_models[1]->SetUVInfo(cyan.uOffset, cyan.vOffset, cyan.uRatio, cyan.vRatio);
	m_models[2]->SetUVInfo(doge.uOffset, doge.vOffset, doge.uRatio, doge.vRatio);
}

void App::PerFrameUpdate() {
	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboard();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::One)) {
		auto doge = Sol::textureAtlas->GetUVInfo("dogeBig");

		m_models[2]->SetUVInfo(doge.uOffset, doge.vOffset, doge.uRatio, doge.vRatio);
	}
	else if(pKeyboardRef->AreKeysPressed(2, SKeyCodes::R, SKeyCodes::One)) {
		auto segs = Sol::textureAtlas->GetUVInfo("segs");

		m_models[2]->SetUVInfo(segs.uOffset, segs.vOffset, segs.uRatio, segs.vRatio);
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
		m_models[1]->GetTransform().MoveTowardsX(-0.1f).MoveTowardsZ(0.3f);

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::S, SKeyCodes::Two))
		m_models[1]->GetTransform().MoveTowardsX(0.1f).MoveTowardsZ(-0.3f);

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

App& App::AddModels(std::vector<std::shared_ptr<Model>>&& models) noexcept {
	for (auto& model : models)
		m_models.emplace_back(std::move(model));

	return *this;
}
