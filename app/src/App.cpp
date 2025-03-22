#include <TextureTools.hpp>
#include <cmath>

#include <App.hpp>
#include <ModelBase.hpp>
#include <BasicMeshBundles.hpp>
#include <CameraManagerSol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <TextureAtlas.hpp>
#include <SceneMaterialProcessor.hpp>
#include <AllocationLiterals.hpp>

static MeshBundleImpl cubeMeshBundle{ true };
static std::uint32_t cubeMeshBundleIndex   = std::numeric_limits<std::uint32_t>::max();
static MeshBundleImpl assimpMeshBundle{ false };
static std::uint32_t assimpMeshBundleIndex   = std::numeric_limits<std::uint32_t>::max();
static MeshBundleImpl sphereMeshBundle{ true };
static std::uint32_t sphereMeshBundleIndex = std::numeric_limits<std::uint32_t>::max();

static std::shared_ptr<ModelBundleBase> cubeBundle1{};
static std::shared_ptr<ModelBundleBase> assimpModelBundle1{};
static std::shared_ptr<ModelBundleBase> assimpModelBundle2{};
static std::shared_ptr<ModelBundleBase> assimpModelBundle3{};
static std::shared_ptr<ModelBundleBase> cubeBundle2{};
static std::shared_ptr<ModelBundleBase> cubeBundle3{};
static std::shared_ptr<ModelBundleBase> cubeBundle4{};
static std::shared_ptr<ModelBundleBase> cubeLightBundle{};
static std::uint32_t cubeBundleIndex1     = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t assimpBundleIndex1   = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t assimpBundleIndex2   = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t assimpBundleIndex3   = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex2     = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex3     = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex4     = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeLightBundleIndex = std::numeric_limits<std::uint32_t>::max();

static std::uint32_t whiteMatIndex      = 0u;
static std::uint32_t tealMatIndex       = std::numeric_limits<std::uint32_t>::max();
static size_t secondTextureIndex        = std::numeric_limits<size_t>::max();
static TextureAtlas atlas{};
static std::uint32_t atlasBindingIndex  = 0u;

static std::uint32_t nonLightPSOIndex   = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t lightPSOIndex      = std::numeric_limits<std::uint32_t>::max();

static std::shared_ptr<PerspectiveCameraEuler> camera{};

App::App(
	Renderer* renderer, InputManager* inputManager, RenderEngineType engineType,
	ExtensionManager* extensionManager, RenderPassManager* renderPassManager, std::uint32_t frameCount
) : m_renderer{ renderer }, m_blinnPhong{ nullptr }, m_inputManager{ inputManager },
	m_engineType{ engineType }, m_renderPassManager{ renderPassManager }
{
	extensionManager->SetBlinnPhongLight(renderer, frameCount);
	//extensionManager->SetWeightedTransparency(renderer);

	//m_renderPassManager->SetTransparencyPass(extensionManager->GetWeightedTransparencySP());

	m_blinnPhong = extensionManager->GetBlinnPhongLight();
}

void App::Init()
{
	{
		Mesh mesh{};

		CubeMesh{}.SetMesh(mesh, CubeUVMode::IndependentFaceTexture);
		cubeMeshBundle.AddMesh(std::move(mesh));

		cubeMeshBundleIndex = m_renderer->AddMeshBundle(cubeMeshBundle.MoveTemporaryData());
	}

	/*
	{
		auto sceneProcessor = std::make_shared<SceneProcessor>(
			"resources/meshes/emd_gp7_western_pacific_713/scene.gltf"
		);

		SceneMaterialProcessor materialProcessor{ sceneProcessor };
		materialProcessor.ProcessMeshAndMaterialData();

		materialProcessor.LoadBlinnPhongMaterials(*m_blinnPhong);
		//materialProcessor.LoadTextures(*m_renderer);
		materialProcessor.LoadTexturesAsAtlas(*m_renderer);

		assimpMeshBundle.SetMeshBundle(sceneProcessor, materialProcessor);

		assimpMeshBundleIndex = m_renderer->AddMeshBundle(assimpMeshBundle.MoveTemporaryData());
	}
	*/
	{
		const ExternalGraphicsPipeline nonLightOpaquePipeline = m_blinnPhong->GetOpaqueLightSrcPipeline(
			m_renderPassManager->GetGraphicsPipelineManager()
		);

		nonLightPSOIndex = m_renderer->AddGraphicsPipeline(nonLightOpaquePipeline);

		const ExternalGraphicsPipeline lightOpaquePipeline = m_blinnPhong->GetOpaqueLightDstPipeline(
			m_renderPassManager->GetGraphicsPipelineManager()
		);

		lightPSOIndex    = m_renderer->AddGraphicsPipeline(lightOpaquePipeline);

		m_renderPassManager->AddPipeline(nonLightPSOIndex);
		m_renderPassManager->AddPipeline(lightPSOIndex);
	}

	cubeLightBundle = std::make_shared<ModelBundleBase>();

	cubeLightBundle->AddModel(0.1f);

	constexpr BlinnPhongLightType lightType = BlinnPhongLightType::Spotlight;

	std::shared_ptr<ModelBase> lightModel   = cubeLightBundle->GetModel(0u);

	lightModel->SetPipelineIndex(nonLightPSOIndex);

	std::uint32_t lightIndex = m_blinnPhong->AddLight(
		std::make_shared<LightSourceWithModel>(std::move(lightModel)), lightType
	);

	std::uint32_t lightIndex1 = m_blinnPhong->AddLight(
		std::make_shared<LightSourceWithoutModel>(), BlinnPhongLightType::Directional
	);

	// Light Properties
	BlinnPhongLightProperties lightProperties
	{
		.ambient   = DirectX::XMFLOAT3{ 0.2f, 0.2f, 0.2f },
		.diffuse   = DirectX::XMFLOAT3{ 0.7f, 0.7f, 0.7f },
		.specular  = DirectX::XMFLOAT3{ 1.f, 1.f, 1.f },
		.lightType = static_cast<std::uint32_t>(lightType)
	};

	switch (lightType)
	{
		case BlinnPhongLightType::Point:
		{
			lightProperties.constant  = 1.f;
			lightProperties.linear    = 0.09f;
			lightProperties.quadratic = 0.032f;

			break;
		}
		case BlinnPhongLightType::Directional:
		{
			lightProperties.direction = DirectX::XMFLOAT3{ 1.f, -0.f, -0.f };

			break;
		}
		case BlinnPhongLightType::Spotlight:
		{
			// Focus direction
			lightProperties.direction = DirectX::XMFLOAT3{ 0.f, 0.f, 1.f };

			lightProperties.constant  = 1.f;
			lightProperties.linear    = 0.09f;
			lightProperties.quadratic = 0.032f;

			// These are done in Cosine, so calculation on the shaders is easier. As we will have
			// to compare the cuttoffs with Dot products.
			lightProperties.innerCutoff = std::cos(DirectX::XMConvertToRadians(12.5f));
			lightProperties.outerCutoff = std::cos(DirectX::XMConvertToRadians(17.5f));

			break;
		}
	}

	m_blinnPhong->SetProperties(lightIndex, lightProperties);

	m_blinnPhong->SetProperties(lightIndex1, lightProperties);
	m_blinnPhong->SetDirection(lightIndex1, DirectX::XMFLOAT3{ 1.f, 0.f, 0.f });
	m_blinnPhong->SetType(lightIndex1, BlinnPhongLightType::Directional);

	camera = std::make_shared<PerspectiveCameraEuler>();

	camera->SetProjectionMatrix(1920u, 1080u);
	camera->SetCameraPosition(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f });

	const std::uint32_t cameraIndex = m_renderer->AddCamera(camera);

	m_renderer->SetCamera(cameraIndex);

	{
		// Green
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Green);
		const BlinnPhongMaterial green
		{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		const size_t materialIndex = m_blinnPhong->AddMaterial(green);
	}

	{
		// White
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::White);
		const BlinnPhongMaterial white
		{
			.ambient   = colourBuffer,
			.diffuse   = colourBuffer,
			.specular  = { 1.f, 1.f, 1.f, 1.f },
			.shininess = 32.f
		};

		const size_t materialIndex = m_blinnPhong->AddMaterial(white);

		whiteMatIndex = static_cast<std::uint32_t>(materialIndex);
	}

	{
		// Orange
		DirectX::XMFLOAT4 colourBuffer{};
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Orange);
		const BlinnPhongMaterial orange
		{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		const size_t materialIndex = m_blinnPhong->AddMaterial(orange);
	}

	{
		atlas.AddTexture("Narrative",    "resources/textures/NTHead.jpg")
			.AddTexture("Panda",         "resources/textures/Panda.png")
			.AddTexture("Unicorn",       "resources/textures/unicorn.jpeg")
			.AddTexture("Katarin",       "resources/textures/Katarin.png")
			.AddTexture("Monika",        "resources/textures/MonikaGun.png")
			.AddTexture("UltraMarine",   "resources/textures/UltraMarine.jpg")
			.AddTexture("Goku",          "resources/textures/Goku.jpg")
			.AddTexture("Container",     "resources/textures/container.png")
			.AddTexture("ContainerSpec", "resources/textures/container_specular.png");

		atlas.CreateAtlas();

		STexture atlastTex = atlas.MoveTexture();

		const size_t textureIndex = m_renderer->AddTexture(std::move(atlastTex));
		atlasBindingIndex         = m_renderer->BindTexture(textureIndex);
	}

	{
		{
			ModelBase& model1 = *cubeLightBundle->GetModel(0u);

			model1.SetMeshIndex(0u);
			model1.SetMaterialIndex(whiteMatIndex);
			model1.SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
			model1.SetDiffuseIndex(atlasBindingIndex);
		}

		cubeLightBundle->SetMeshBundleIndex(cubeMeshBundleIndex);

		cubeLightBundleIndex = m_renderer->AddModelBundle(
			std::make_shared<ModelBundleImpl>(cubeLightBundle)
		);

		m_renderPassManager->AddModelBundle(cubeLightBundleIndex);
	}

	{
		cubeBundle1 = std::make_shared<ModelBundleBase>();

		cubeBundle1->AddModel(0.45f).AddModel(0.45f);

		// We have only a single mesh in the bundle.
		{
			ModelBase& model1 = *cubeBundle1->GetModel(0u);

			model1.SetMeshIndex(0u);
			model1.SetMaterialIndex(whiteMatIndex);
			model1.SetDiffuseUVInfo(atlas.GetUVInfo("Container"));
			model1.SetDiffuseIndex(atlasBindingIndex);
			model1.SetSpecularUVInfo(atlas.GetUVInfo("ContainerSpec"));
			model1.SetSpecularIndex(atlasBindingIndex);
			model1.SetPipelineIndex(lightPSOIndex);

			cubeBundle1->MoveTowardsX(0u, 0.75f);
		}

		{
			ModelBase& model2 = *cubeBundle1->GetModel(1u);

			model2.SetMeshIndex(0u);
			model2.SetMaterialIndex(whiteMatIndex);
			model2.SetDiffuseUVInfo(atlas.GetUVInfo("Katarin"));
			model2.SetDiffuseIndex(atlasBindingIndex);
			model2.SetSpecularUVInfo(atlas.GetUVInfo("Katarin"));
			model2.SetSpecularIndex(atlasBindingIndex);
			model2.SetPipelineIndex(lightPSOIndex);

			cubeBundle1->MoveTowardsY(1u, 0.75f);
		}

		cubeBundle1->SetMeshBundleIndex(cubeMeshBundleIndex);

		cubeBundleIndex1 = m_renderer->AddModelBundle(
			std::make_shared<ModelBundleImpl>(cubeBundle1)
		);

		m_renderPassManager->AddModelBundle(cubeBundleIndex1);
	}

	/*
	{
		assimpModelBundle1.SetMeshBundle(assimpMeshBundleIndex, 0.5f, assimpMeshBundle);
		assimpModelBundle1.MoveTowardsZ(0u, 1.f);
		assimpModelBundle1.SetMeshBundleIndex(assimpMeshBundleIndex);
		assimpBundleIndex1 = assimpModelBundle1.SetModelBundle(*m_renderer, L"TestFragmentShader");
	}

	{
		assimpModelBundle2.SetMeshBundle(assimpMeshBundleIndex, 0.5f, assimpMeshBundle);
		assimpModelBundle2.MoveTowardsZ(0u, 1.f).MoveTowardsX(0u, 10.f);
		assimpModelBundle2.SetMeshBundleIndex(assimpMeshBundleIndex);
		assimpBundleIndex2 = assimpModelBundle2.SetModelBundle(*m_renderer, L"TestFragmentShader");
	}

	{
		assimpModelBundle3.SetMeshBundle(assimpMeshBundleIndex, 0.5f, assimpMeshBundle);
		assimpModelBundle3.MoveTowardsZ(0u, 1.f).MoveTowardsX(0u, -10.f);
		assimpModelBundle3.SetMeshBundleIndex(assimpMeshBundleIndex);
		assimpBundleIndex3 = assimpModelBundle3.SetModelBundle(*m_renderer, L"TestFragmentShader");
	}
	*/
}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	const Keyboard& keyboard = m_inputManager->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::UpArrow))
		cubeLightBundle->MoveTowardsZ(0u, 0.05f);

	if (keyboard.IsKeyPressed(SKeyCodes::DownArrow))
		cubeLightBundle->MoveTowardsZ(0u, -0.05f);

	if (keyboard.IsKeyPressed(SKeyCodes::RightArrow))
		cubeLightBundle->MoveTowardsX(0u, 0.05f);

	if (keyboard.IsKeyPressed(SKeyCodes::LeftArrow))
		cubeLightBundle->MoveTowardsX(0u, -0.05f);

	if (keyboard.IsKeyPressed(SKeyCodes::W))
		camera->MoveForward(0.01f).MoveCamera();

	if (keyboard.IsKeyPressed(SKeyCodes::S))
		camera->MoveBackward(0.01f).MoveCamera();

	//if (keyboard.IsKeyPressed(SKeyCodes::E))
	//	assimpModelBundle1->RotatePitchDegree(0u, 1.f);

	//if (keyboard.IsKeyPressed(SKeyCodes::Q))
	//	assimpModelBundle1->RotatePitchDegree(0u, -1.f);

	if (keyboard.IsKeyPressed(SKeyCodes::D))
		cubeBundle1->MoveTowardsX(0u, 0.01f);

	if (keyboard.IsKeyPressed(SKeyCodes::A))
		cubeBundle1->MoveTowardsX(0u, -0.01f);

	if (keyboard.IsKeyPressed(SKeyCodes::C))
	{
		if (cubeBundleIndex2 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle2 = std::make_shared<ModelBundleBase>();

			cubeBundle2->AddModel(0.45f).AddModel(0.45f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle2->GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(1u);
				model1.SetDiffuseIndex(atlasBindingIndex);
				model1.SetSpecularIndex(atlasBindingIndex);
				model1.SetPipelineIndex(lightPSOIndex);
			}

			{
				ModelBase& model2 = *cubeBundle2->GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Monika"));
				model2.SetDiffuseIndex(atlasBindingIndex);
				model2.SetSpecularUVInfo(atlas.GetUVInfo("Monika"));
				model2.SetSpecularIndex(atlasBindingIndex);
				model2.SetPipelineIndex(lightPSOIndex);

				cubeBundle2->MoveTowardsX(1u, -0.75f);
			}

			cubeBundle2->SetMeshBundleIndex(cubeMeshBundleIndex);

			cubeBundleIndex2 = m_renderer->AddModelBundle(
				std::make_shared<ModelBundleImpl>(cubeBundle2)
			);

			m_renderPassManager->AddModelBundle(cubeBundleIndex2);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex2);

			m_renderPassManager->RemoveModelBundle(cubeBundleIndex2);

			cubeBundleIndex2 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle2.reset();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::V))
	{
		if (cubeBundleIndex3 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle3 = std::make_shared<ModelBundleBase>();

			cubeBundle3->AddModel(0.45f).AddModel(0.45f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle3->GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(whiteMatIndex);
				model1.SetDiffuseUVInfo(atlas.GetUVInfo("Unicorn"));
				model1.SetDiffuseIndex(atlasBindingIndex);
				model1.SetSpecularUVInfo(atlas.GetUVInfo("Unicorn"));
				model1.SetSpecularIndex(atlasBindingIndex);
				model1.SetPipelineIndex(lightPSOIndex);

				cubeBundle3->MoveModel(0u, DirectX::XMFLOAT3{ -1.4f, -0.75f, 0.f });
			}

			{
				ModelBase& model2 = *cubeBundle3->GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Panda"));
				model2.SetDiffuseIndex(atlasBindingIndex);
				model2.SetSpecularUVInfo(atlas.GetUVInfo("Panda"));
				model2.SetSpecularIndex(atlasBindingIndex);
				model2.SetPipelineIndex(lightPSOIndex);

				cubeBundle3->MoveModel(1u, DirectX::XMFLOAT3{ -0.45f, -0.75f, 0.f });
			}

			cubeBundle3->SetMeshBundleIndex(cubeMeshBundleIndex);

			cubeBundleIndex3 = m_renderer->AddModelBundle(
				std::make_shared<ModelBundleImpl>(cubeBundle3)
			);

			m_renderPassManager->AddModelBundle(cubeBundleIndex3);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (cubeBundleIndex3 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex3);
			m_renderPassManager->RemoveModelBundle(cubeBundleIndex3);

			cubeBundleIndex3 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle3.reset();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::N))
	{
		if (cubeBundleIndex4 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle4 = std::make_shared<ModelBundleBase>();

			cubeBundle4->AddModel(0.45f).AddModel(0.45f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle4->GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(whiteMatIndex);
				model1.SetDiffuseUVInfo(atlas.GetUVInfo("UltraMarine"));
				model1.SetDiffuseIndex(atlasBindingIndex);
				model1.SetSpecularUVInfo(atlas.GetUVInfo("UltraMarine"));
				model1.SetSpecularIndex(atlasBindingIndex);
				model1.SetPipelineIndex(lightPSOIndex);

				cubeBundle4->MoveModel(0u, DirectX::XMFLOAT3{ 1.4f, -0.75f, 0.f });
			}

			{
				ModelBase& model2 = *cubeBundle4->GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Goku"));
				model2.SetDiffuseIndex(atlasBindingIndex);
				model2.SetSpecularUVInfo(atlas.GetUVInfo("Goku"));
				model2.SetSpecularIndex(atlasBindingIndex);
				model2.SetPipelineIndex(lightPSOIndex);

				cubeBundle4->MoveModel(1u, DirectX::XMFLOAT3{ 0.45f, -0.75f, 0.f });
			}

			cubeBundle4->SetMeshBundleIndex(cubeMeshBundleIndex);

			cubeBundleIndex4 = m_renderer->AddModelBundle(
				std::make_shared<ModelBundleImpl>(cubeBundle4)
			);

			m_renderPassManager->AddModelBundle(cubeBundleIndex4);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (cubeBundleIndex4 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex4);
			m_renderPassManager->RemoveModelBundle(cubeBundleIndex4);

			cubeBundleIndex4 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle4.reset();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::F))
	{
		if (tealMatIndex == std::numeric_limits<std::uint32_t>::max())
		{
			DirectX::XMFLOAT4 colourBuffer{};
			DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Teal);
			const BlinnPhongMaterial teal
			{
				.ambient  = colourBuffer,
				.diffuse  = colourBuffer,
				.specular = { 1.f, 1.f, 1.f, 1.f }
			};

			tealMatIndex = m_blinnPhong->AddMaterial(teal);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::G))
	{
		if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
		{
			std::uint32_t index = whiteMatIndex;

			if (tealMatIndex != std::numeric_limits<std::uint32_t>::max())
				index = tealMatIndex;

			ModelBase& model1 = *cubeBundle2->GetModel(0u);

			model1.SetMaterialIndex(index);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::R))
	{
		if (tealMatIndex != std::numeric_limits<std::uint32_t>::max())
		{
			m_blinnPhong->RemoveMaterial(tealMatIndex);

			tealMatIndex = std::numeric_limits<std::uint32_t>::max();
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::O))
	{
		if (sphereMeshBundleIndex == std::numeric_limits<std::uint32_t>::max())
		{
			Mesh mesh{};

			SphereMesh{ 64u, 64u }.SetMesh(mesh);
			sphereMeshBundle.AddMesh(std::move(mesh));

			sphereMeshBundleIndex = m_renderer->AddMeshBundle(sphereMeshBundle.MoveTemporaryData());
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::P))
	{
		const bool bundle1Exists = cubeBundleIndex1 != std::numeric_limits<std::uint32_t>::max();
		const bool sphereExists  = sphereMeshBundleIndex != std::numeric_limits<std::uint32_t>::max();

		if (bundle1Exists && sphereExists)
		{
			ModelBase& model1 = *cubeBundle1->GetModel(0u);

			model1.SetMeshIndex(0u);
			model1.SetDiffuseIndex(0u);
			model1.SetSpecularIndex(0u);
			model1.SetDiffuseUVInfo(UVInfo{});
			model1.SetSpecularUVInfo(UVInfo{});

			ModelBase& model2 = *cubeBundle1->GetModel(1u);

			model2.SetMeshIndex(0u);
			model2.SetDiffuseIndex(0u);
			model2.SetSpecularIndex(0u);
			model2.SetDiffuseUVInfo(UVInfo{});
			model2.SetSpecularUVInfo(UVInfo{});

			cubeBundle1->SetMeshBundleIndex(sphereMeshBundleIndex);
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
				const size_t textureIndex = m_renderer->AddTexture(std::move(texture));

				secondTextureIndex = m_renderer->BindTexture(textureIndex);
			}
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Three))
	{
		if (secondTextureIndex != std::numeric_limits<size_t>::max())
		{
			ModelBase& model1 = *cubeBundle1->GetModel(0u);

			model1.SetDiffuseUVInfo(UVInfo{});
			model1.SetDiffuseIndex(secondTextureIndex);
		}
	}
}
