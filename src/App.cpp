#include <TextureLoader.hpp>

#include <App.hpp>
#include <BasicModels.hpp>
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

	auto cube0 = std::make_shared<OneThirdModel>();
	auto cube1 = std::make_shared<OneThirdModel>();
	auto sphere0 = std::make_shared<ModelWithArrow>();

	cube0->SetTextureIndex(0u);
	cube1->SetTextureIndex(0u);
	sphere0->SetTextureIndex(0u);

	cube1->GetTransform().RotateYDegree(45.f).MoveTowardsZ(-0.5f).MoveTowardsX(-1.5f);
	cube0->GetTransform().RotateYDegree(45.f);
	sphere0->GetTransform().MoveTowardsX(2.5f);

	cube0->SetTextureName("Fuchsia");
	cube1->SetTextureName("Cyan");
	sphere0->SetTextureName("Green");

	std::wstring pixelShader0 = L"PixelShader";

	Sol::modelContainer->AddModel<CubeInputs>(std::move(cube0), pixelShader0);
	Sol::modelContainer->AddModel<CubeInputs>(std::move(cube1), pixelShader0);
	Sol::modelContainer->AddModel<SphereInputs>(std::move(sphere0), { 64u, 64u }, pixelShader0);
}

void App::SetResources() {}

void App::PerFrameUpdate() {}

void App::PhysicsUpdate() {
	constexpr float cameraMoveSpeed = 0.001f;

	IKeyboard* pKeyboardRef = Sol::ioMan->GetKeyboard();

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::W)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveForward(cameraMoveSpeed).MoveCamera();
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::S)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveBackward(cameraMoveSpeed).MoveCamera();
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::A)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveLeft(cameraMoveSpeed).MoveCamera();
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::D)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveRight(cameraMoveSpeed).MoveCamera();
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::T)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveUp(cameraMoveSpeed).MoveCamera();
	}

	if (pKeyboardRef->IsKeyPressed(SKeyCodes::G)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveDown(cameraMoveSpeed).MoveCamera();
	}

	IGamepad* pGamepadRef = Sol::ioMan->GetGamepad();

	if (auto thumbStickData = pGamepadRef->ReadRightThumbStickData(); thumbStickData) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		auto& [magnitude, offsetX, offsetY] = *thumbStickData;

		camera->LookAround(offsetX * 0.001f, -1.f * offsetY * 0.0001f);
	}

	AMods::cameraManager->SetCamera();
}
