#include <TextureTools.hpp>

#include <App.hpp>
#include <Sol.hpp>
#include <MaterialBase.hpp>
#include <BasicModels.hpp>
#include <BasicMeshBundles.hpp>
#include <CameraManagerSol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <TextureAtlas.hpp>
#include <ModelProcessor.hpp>

static std::shared_ptr<ModelBundleBaseVS> sCubeBundle{};
static std::uint32_t cubeBundleIndex1 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle2{};
static std::uint32_t cubeBundleIndex2 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle3{};
static std::uint32_t cubeBundleIndex3 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle4{};
static std::uint32_t cubeBundleIndex4 = 0u;

static std::shared_ptr<MaterialBase> sTeal{};
static std::uint32_t cubeIndexCount   = 0u;
static std::uint32_t sphereIndexCount = 0u;
static std::uint32_t whiteMatIndex    = 0u;
static bool isSphereAdded             = false;
static size_t secondTextureIndex      = std::numeric_limits<size_t>::max();
static RenderEngineType engineType    = RenderEngineType::IndividualDraw;
static TextureAtlas atlas{};
static std::uint32_t atlasBindingIndex = 0u;

App::App()
{
	auto cubeMesh = std::make_unique<MeshBaseVSWrapper<CubeMesh>>(CubeUVMode::IndependentFaceTexture);
	auto camera   = std::make_shared<PerspectiveCameraEuler>();

	camera->SetProjectionMatrix(1920u, 1080u);
	camera->SetCameraPosition(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f });

	cubeIndexCount = static_cast<std::uint32_t>(std::size(cubeMesh->GetIndices()));

	engineType = Sol::configManager->GetRenderEngineType();

	if (engineType == RenderEngineType::IndirectDraw)
		cubeMesh->SetBounds(BoundType::Rectangle);

	const std::uint32_t meshIndex   = Sol::renderer->AddMeshBundle(std::move(cubeMesh));

	const std::uint32_t cameraIndex = Sol::renderer->AddCamera(camera);

	Sol::renderer->SetCamera(cameraIndex);

	{
		// Green
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Green);
		const MaterialData green{
			.ambient = colourBuffer,
			.diffuse = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto greenMat = std::make_shared<MaterialBase>();
		greenMat->SetData(green);

		const size_t materialIndex = Sol::renderer->AddMaterial(std::move(greenMat));
	}

	{
		// White
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::White);
		const MaterialData white{
			.ambient = colourBuffer,
			.diffuse = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto whiteMat = std::make_shared<MaterialBase>();
		whiteMat->SetData(white);

		const size_t materialIndex = Sol::renderer->AddMaterial(std::move(whiteMat));

		whiteMatIndex = static_cast<std::uint32_t>(materialIndex);
	}

	{
		// Orange
		DirectX::XMFLOAT4 colourBuffer{};
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Orange);
		const MaterialData orange{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto orangeMat = std::make_shared<MaterialBase>();
		orangeMat->SetData(orange);

		const size_t materialIndex = Sol::renderer->AddMaterial(std::move(orangeMat));
	}

	{
		auto testTexture  = TextureTool::LoadTextureFromFile("resources/textures/NTHead.jpg");
		auto testTexture2 = TextureTool::LoadTextureFromFile("resources/textures/Panda.png");
		auto testTexture3 = TextureTool::LoadTextureFromFile("resources/textures/unicorn.jpeg");
		auto testTexture4 = TextureTool::LoadTextureFromFile("resources/textures/Katarin.png");
		auto testTexture5 = TextureTool::LoadTextureFromFile("resources/textures/MonikaGun.png");
		auto testTexture6 = TextureTool::LoadTextureFromFile("resources/textures/UltraMarine.jpg");
		auto testTexture7 = TextureTool::LoadTextureFromFile("resources/textures/Goku.jpg");

		if (testTexture && testTexture2 && testTexture3 &&
			testTexture4 && testTexture5 && testTexture6 && testTexture7)
		{
			STexture& texture  = testTexture.value();
			STexture& texture2 = testTexture2.value();
			STexture& texture3 = testTexture3.value();
			STexture& texture4 = testTexture4.value();
			STexture& texture5 = testTexture5.value();
			STexture& texture6 = testTexture6.value();
			STexture& texture7 = testTexture7.value();

			atlas.AddTexture("Narrative", std::move(texture));
			atlas.AddTexture("Panda", std::move(texture2));
			atlas.AddTexture("Unicorn", std::move(texture3));
			atlas.AddTexture("Katarin", std::move(texture4));
			atlas.AddTexture("Monika", std::move(texture5));
			atlas.AddTexture("UltraMarine", std::move(texture6));
			atlas.AddTexture("Goku", std::move(texture7));

			atlas.CreateAtlas();

			STexture atlastTex = atlas.MoveTexture();

			const size_t textureIndex = Sol::renderer->AddTexture(std::move(atlastTex));
			atlasBindingIndex         = Sol::renderer->BindTexture(textureIndex);
		}
	}

	{
		auto quadMesh = std::make_unique<MeshBaseVSWrapper<QuadMesh>>();

		const std::vector<std::uint32_t>& indices = quadMesh->GetIndices();

		std::vector<std::uint32_t> vertexIndices{};
		std::vector<PrimitiveIndices> primIndices{};

		Meshlet meshlet = MakeMeshlet(indices, 0u, std::size(indices), 0u, vertexIndices, primIndices);
	}

	auto cubeBundle = std::make_shared<ModelBundleBaseVS>();

	{
		auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

		cubeBundle->AddModel(cube);

		cube->SetMeshDetailsVS(
			MeshDetailsVS{
				.indexCount  = cubeIndexCount,
				.indexOffset = 0u
			}
		);

		cube->GetBase().SetMaterialIndex(whiteMatIndex);
		cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
		cube->GetBase().SetDiffuseIndex(atlasBindingIndex);
	}

	{
		auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

		cubeBundle->AddModel(cube);

		cube->SetMeshDetailsVS(
			MeshDetailsVS{
				.indexCount  = cubeIndexCount,
				.indexOffset = 0u
			}
		);

		cube->GetBase().SetMaterialIndex(whiteMatIndex);
		cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Katarin"));
		cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

		cube->GetBase().GetTransform().MoveTowardsY(1.2f);
	}

	cubeBundleIndex1 = Sol::renderer->AddModelBundle(
		cubeBundle->GetBundleImpl(), L"TestFragmentShader"
	);

	sCubeBundle = cubeBundle;

	/*
	TextureTool::AddTextureToAtlas("resources/textures/segs.jpg", "segs");
	TextureTool::AddTextureToAtlas("resources/textures/doge1.jpg", "doge");
	TextureTool::AddTextureToAtlas("resources/textures/doge.jpg", "dogeBig");

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

	auto cube0 = std::make_shared<ScalableModel>(0.3f);
	auto cube1 = std::make_shared<ScalableModel>(0.3f);
	auto sphere0 = std::make_shared<OrbitModelClock>(0.1f);
	auto sphere1 = std::make_shared<OrbitModelAntiClock>(0.1f);
	auto sphere2 = std::make_shared<OrbitModelClock>(0.1f);
	auto sphere3 = std::make_shared<OrbitModelClock>(0.1f);
	auto sphere4 = std::make_shared<OrbitModelClock>(0.1f);
	auto quad0 = std::make_shared<ScalableModel>(0.5f);

	cube1->GetTransform().RotateYawDegree(45.f).MoveTowardsZ(-0.5f).MoveTowardsX(-1.1f);
	cube0->GetTransform().RotateYawDegree(45.f);
	quad0->GetTransform().MoveTowardsX(1.5f);

	sphere0->GetTransform().MoveTowardsX(8.f);
	sphere0->MeasureRadius();

	sphere1->GetTransform().MoveTowardsX(6.5f);
	sphere1->MeasureRadius();

	sphere2->GetTransform().MoveTowardsY(5.5f);
	sphere2->MeasureRadius();

	sphere3->GetTransform().MoveTowardsX(6.5f).MoveTowardsY(6.5f);
	sphere3->MeasureRadius();

	sphere4->GetTransform().MoveTowardsX(3.5f).MoveTowardsY(-5.5f);
	sphere4->MeasureRadius();

	TextureTool::AddTextureToAtlas("resources/textures/container2.png", "Box");
	cube0->SetTextureFromAtlas("Box", TextureType::diffuse);
	TextureTool::AddTextureToAtlas("resources/textures/container2_specular.png", "BoxSpec");
	cube1->SetTextureFromAtlas("BoxSpec", TextureType::specular);

	auto boxTex = TextureTool::LoadTextureFromFile("resources/textures/container2.png");
	auto boxSpecTex = TextureTool::LoadTextureFromFile(
		"resources/textures/container2_specular.png"
	);
	if (boxTex) {
		STexture& box = boxTex.value();
		size_t texIndex = Sol::renderer->AddTexture(
			std::move(box.data), box.width, box.height
		);

		//cube1->SetDiffuseTexIndex(texIndex);
	}
	if (boxSpecTex) {
		STexture& boxSpec = boxSpecTex.value();
		size_t texIndex = Sol::renderer->AddTexture(
			std::move(boxSpec.data), boxSpec.width, boxSpec.height
		);

		//cube0->SetSpecularTexIndex(texIndex);
	}

	// Lights
	DirectX::XMFLOAT4 colourBuffer{};
	DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Green);
	const MaterialData green{
		.ambient  = colourBuffer,
		.diffuse  = colourBuffer,
		.specular = { 1.f, 1.f, 1.f, 1.f }
	};
	//sphere0->SetMaterial(green);

	const MaterialData white{
		.ambient  = { 0.2f, 0.2f, 0.2f, 1.f },
		.diffuse  = { 0.5f, 0.5f, 0.5f, 1.f },
		.specular = { 1.f, 1.f, 1.f, 1.f }
	};
	//sphere1->SetMaterial(white);
	sphere1->SetAsLightSource();

	DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Red);
	const MaterialData red{
		.ambient  = colourBuffer,
		.diffuse  = colourBuffer,
		.specular = { 1.f, 1.f, 1.f, 1.f }
	};
	//sphere2->SetMaterial(red);

	DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Yellow);
	const MaterialData yellow{
		.ambient  = colourBuffer,
		.diffuse  = colourBuffer,
		.specular = { 1.f, 1.f, 1.f, 1.f }
	};
	//sphere3->SetMaterial(yellow);

	DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Orange);
	const MaterialData orange{
		.ambient  = colourBuffer,
		.diffuse  = colourBuffer,
		.specular = { 1.f, 1.f, 1.f, 1.f }
	};
	//sphere4->SetMaterial(orange);

	std::wstring lightShader = L"LightShader";
	std::wstring withoutLightShader = L"WithoutLightShader";

	Sol::modelContainer->AddModel<CubeInputs>(
		std::move(cube0), { CubeUVMode::IndependentFaceTexture }, lightShader
	);
	Sol::modelContainer->AddModel<CubeInputs>(
		std::move(cube1), { CubeUVMode::IndependentFaceTexture }, lightShader
	);
	Sol::modelContainer->AddModel<SphereInputs>(
		std::move(sphere0), { 64u, 64u }, withoutLightShader
		);
	Sol::modelContainer->AddModel<SphereInputs>(
		std::move(sphere1), { 64u, 64u }, withoutLightShader
		);
	Sol::modelContainer->AddModel<SphereInputs>(
		std::move(sphere2), { 64u, 64u }, withoutLightShader
		);
	Sol::modelContainer->AddModel<SphereInputs>(
		std::move(sphere3), { 64u, 64u }, withoutLightShader
		);
	Sol::modelContainer->AddModel<SphereInputs>(
		std::move(sphere4), { 16u, 16u }, withoutLightShader
		);

	auto dogeTex = TextureTool::LoadTextureFromFile("resources/textures/doge.jpg");
	if (dogeTex) {
		STexture& dogeBig = dogeTex.value();
		size_t texIndex = Sol::renderer->AddTexture(
			std::move(dogeBig.data), dogeBig.width, dogeBig.height
		);

		//quad0->SetDiffuseTexIndex(texIndex);
	}

	Sol::modelContainer->AddModel<QuadInputs>(std::move(quad0), withoutLightShader);
	*/
}

void App::SetResources() {}

void App::PerFrameUpdate() {}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	IKeyboard& keyboard = Sol::ioMan->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::D))
	{
		auto& cube = sCubeBundle->GetModel(0u);
		cube->GetBase().GetTransform().MoveTowardsX(0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::A))
	{
		auto& cube = sCubeBundle->GetModel(0u);
		cube->GetBase().GetTransform().MoveTowardsX(-0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::C))
	{
		if (!sCubeBundle2)
		{
			auto cubeBundle = std::make_shared<ModelBundleBaseVS>();
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cubeBundle->AddModel(std::move(cube));
			}
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cube->GetBase().SetMaterialIndex(whiteMatIndex);
				cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Monika"));
				cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

				cube->GetBase().GetTransform().MoveTowardsX(-1.5f);

				cubeBundle->AddModel(std::move(cube));
			}

			cubeBundleIndex2 = Sol::renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle2 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (sCubeBundle2)
		{
			Sol::renderer->RemoveModelBundle(cubeBundleIndex2);

			sCubeBundle2.reset();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::V))
	{
		if (!sCubeBundle3)
		{
			auto cubeBundle = std::make_shared<ModelBundleBaseVS>();
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cube->GetBase().SetMaterialIndex(whiteMatIndex);
				cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Unicorn"));
				cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

				cube->GetBase().GetTransform().MoveTowardsX(-2.4f).MoveTowardsY(-1.2f);

				cubeBundle->AddModel(std::move(cube));
			}
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cube->GetBase().SetMaterialIndex(whiteMatIndex);
				cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Panda"));
				cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

				cube->GetBase().GetTransform().MoveTowardsX(-0.8f).MoveTowardsY(-1.2f);

				cubeBundle->AddModel(std::move(cube));
			}

			cubeBundleIndex3 = Sol::renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle3 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (sCubeBundle3)
		{
			Sol::renderer->RemoveModelBundle(cubeBundleIndex3);

			sCubeBundle3.reset();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::N))
	{
		if (!sCubeBundle4)
		{
			auto cubeBundle = std::make_shared<ModelBundleBaseVS>();
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cube->GetBase().SetMaterialIndex(whiteMatIndex);
				cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("UltraMarine"));
				cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

				cube->GetBase().GetTransform().MoveTowardsX(2.4f).MoveTowardsY(-1.2f);

				cubeBundle->AddModel(std::move(cube));
			}
			{
				auto cube = std::make_shared<ModelBaseVS>(ModelBase{ 0.3f });

				cube->SetMeshDetailsVS(
					MeshDetailsVS{
						.indexCount  = cubeIndexCount,
						.indexOffset = 0u
					}
				);

				cube->GetBase().SetMaterialIndex(1u);

				cube->GetBase().SetMaterialIndex(whiteMatIndex);
				cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Goku"));
				cube->GetBase().SetDiffuseIndex(atlasBindingIndex);

				cube->GetBase().GetTransform().MoveTowardsX(0.8f).MoveTowardsY(-1.2f);

				cubeBundle->AddModel(std::move(cube));
			}

			cubeBundleIndex4 = Sol::renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle4 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (sCubeBundle4)
		{
			Sol::renderer->RemoveModelBundle(cubeBundleIndex4);

			sCubeBundle4.reset();
		}
	}


	if (keyboard.IsKeyPressed(SKeyCodes::F))
	{
		DirectX::XMFLOAT4 colourBuffer{};
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Teal);
		const MaterialData teal{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto tealMat = std::make_shared<MaterialBase>();
		tealMat->SetData(teal);

		const size_t materialIndex = Sol::renderer->AddMaterial(tealMat);

		sTeal = std::move(tealMat);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Q))
	{
		if (sCubeBundle2)
		{
			std::uint32_t index = 2u;
			if (sTeal)
				index = 3u;

			auto& cube = sCubeBundle2->GetModel(0u);

			cube->GetBase().SetMaterialIndex(index);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::R))
	{
		sTeal.reset();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::O))
	{
		if (!isSphereAdded)
		{
			auto sphereMesh = std::make_unique<MeshBaseVSWrapper<SphereMesh>>(64u, 64u);

			sphereIndexCount = static_cast<std::uint32_t>(std::size(sphereMesh->GetIndices()));

			if (engineType == RenderEngineType::IndirectDraw)
				sphereMesh->SetBounds(BoundType::Rectangle);

			const std::uint32_t sphereIndex = Sol::renderer->AddMeshBundle(std::move(sphereMesh));

			isSphereAdded = true;
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::P))
	{
		if (sCubeBundle && isSphereAdded)
		{
			std::uint32_t index = 1u;

			auto& cube = sCubeBundle->GetModel(0u);

			cube->SetMeshDetailsVS(
				MeshDetailsVS{
					.indexCount  = sphereIndexCount,
					.indexOffset = 0u
				}
			);

			sCubeBundle->SetMeshIndex(index);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Two))
	{
		if (secondTextureIndex == std::numeric_limits<size_t>::max())
		{
			auto testTexure = TextureTool::LoadTextureFromFile("resources/textures/Panda.png");

			if (testTexure)
			{
				STexture& texture         = testTexure.value();
				const size_t textureIndex = Sol::renderer->AddTexture(std::move(texture));

				secondTextureIndex = Sol::renderer->BindTexture(textureIndex);
			}
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Three))
	{
		if (secondTextureIndex != std::numeric_limits<size_t>::max())
		{
			auto& cube = sCubeBundle->GetModel(0u);
			cube->GetBase().SetDiffuseUVInfo(UVInfo{});
			cube->GetBase().SetDiffuseIndex(secondTextureIndex);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Four))
	{
		Sol::renderer->UnbindTexture(0u);
	}

	/*
	constexpr float cameraMoveSpeed = 0.001f;

	IKeyboard& keyboard = Sol::ioMan->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::W)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveForward(cameraMoveSpeed).MoveCamera();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::S)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveBackward(cameraMoveSpeed).MoveCamera();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::A)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveLeft(cameraMoveSpeed).MoveCamera();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::D)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveRight(cameraMoveSpeed).MoveCamera();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::T)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveUp(cameraMoveSpeed).MoveCamera();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::G)) {
		auto camera = AMods::cameraManager->GetFirstEulerCamera();

		camera->MoveDown(cameraMoveSpeed).MoveCamera();
	}

	IGamepad& gamepad = Sol::ioMan->GetGamepad();

	if (auto thumbStickData = gamepad.ReadRightThumbStickData(); thumbStickData) {
		auto camera = AMods::cameraManager->GetEulerCamera(0u);

		auto& [magnitude, offsetX, offsetY] = *thumbStickData;

		camera->LookAround(offsetX * 0.001f, -1.f * offsetY * 0.0001f);
	}

	AMods::cameraManager->SetCamera();
	*/
}
