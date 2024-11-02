#include <TextureTools.hpp>

#include <App.hpp>
#include <MaterialBase.hpp>
#include <ModelBase.hpp>
#include <BasicMeshBundles.hpp>
#include <CameraManagerSol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <TextureAtlas.hpp>
#include <MeshBounds.hpp>

static std::uint32_t cubeMeshBundleIndex   = std::numeric_limits<std::uint32_t>::max();;
static std::uint32_t sphereMeshBundleIndex = std::numeric_limits<std::uint32_t>::max();;

static ModelBundleBase cubeBundle1{};
static ModelBundleBase cubeBundle2{};
static ModelBundleBase cubeBundle3{};
static ModelBundleBase cubeBundle4{};
static std::uint32_t cubeBundleIndex1 = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex2 = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex3 = std::numeric_limits<std::uint32_t>::max();
static std::uint32_t cubeBundleIndex4 = std::numeric_limits<std::uint32_t>::max();

static std::shared_ptr<MaterialBase> sTeal{};
static std::uint32_t whiteMatIndex      = 0u;
static size_t secondTextureIndex        = std::numeric_limits<size_t>::max();
static TextureAtlas atlas{};
static std::uint32_t atlasBindingIndex  = 0u;

void App::Init()
{
	{
		MeshBundleBase cubeMesh{};

		Mesh mesh{};

		CubeMesh{}.SetMesh(mesh, CubeUVMode::IndependentFaceTexture);

		if (m_engineType == RenderEngineType::IndirectDraw)
			mesh.bounds = RectangleBounds{}.GetBounds(mesh.vertices);

		cubeMesh.AddMesh(std::move(mesh));

		cubeMeshBundleIndex = MeshBundleBase::SetMeshBundle(*m_renderer, std::move(cubeMesh));
	}

	auto camera   = std::make_shared<PerspectiveCameraEuler>();

	camera->SetProjectionMatrix(1920u, 1080u);
	camera->SetCameraPosition(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f });

	const std::uint32_t cameraIndex = m_renderer->AddCamera(camera);

	m_renderer->SetCamera(cameraIndex);

	{
		// Green
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::Green);
		const MaterialData green{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto greenMat = std::make_shared<MaterialBase>();
		greenMat->SetData(green);

		const size_t materialIndex = m_renderer->AddMaterial(std::move(greenMat));
	}

	{
		// White
		DirectX::XMFLOAT4 colourBuffer{ 1.f, 0.f, 0.f, 1.f };
		DirectX::XMStoreFloat4(&colourBuffer, DirectX::Colors::White);
		const MaterialData white{
			.ambient  = colourBuffer,
			.diffuse  = colourBuffer,
			.specular = { 1.f, 1.f, 1.f, 1.f }
		};

		auto whiteMat = std::make_shared<MaterialBase>();
		whiteMat->SetData(white);

		const size_t materialIndex = m_renderer->AddMaterial(std::move(whiteMat));

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

		const size_t materialIndex = m_renderer->AddMaterial(std::move(orangeMat));
	}

	{
		atlas.AddTexture("Narrative",  "resources/textures/NTHead.jpg")
			.AddTexture("Panda",       "resources/textures/Panda.png")
			.AddTexture("Unicorn",     "resources/textures/unicorn.jpeg")
			.AddTexture("Katarin",     "resources/textures/Katarin.png")
			.AddTexture("Monika",      "resources/textures/MonikaGun.png")
			.AddTexture("UltraMarine", "resources/textures/UltraMarine.jpg")
			.AddTexture("Goku",        "resources/textures/Goku.jpg");

		atlas.CreateAtlas();

		STexture atlastTex = atlas.MoveTexture();

		const size_t textureIndex = m_renderer->AddTexture(std::move(atlastTex));
		atlasBindingIndex         = m_renderer->BindTexture(textureIndex);
	}

	{
		cubeBundle1.AddModel(0.3f).AddModel(0.3f);

		// We have only a single mesh in the bundle.
		{
			ModelBase& model1 = *cubeBundle1.GetModel(0u);

			model1.SetMeshIndex(0u);
			model1.SetMaterialIndex(whiteMatIndex);
			model1.SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
			model1.SetDiffuseIndex(atlasBindingIndex);
		}

		{
			ModelBase& model2 = *cubeBundle1.GetModel(1u);

			model2.SetMeshIndex(0u);
			model2.SetMaterialIndex(whiteMatIndex);
			model2.SetDiffuseUVInfo(atlas.GetUVInfo("Katarin"));
			model2.SetDiffuseIndex(atlasBindingIndex);

			model2.GetTransform().MoveTowardsY(1.2f);
		}

		cubeBundle1.SetMeshBundleIndex(cubeMeshBundleIndex);
		cubeBundleIndex1 = cubeBundle1.SetModelBundle(*m_renderer, L"TestFragmentShader");
	}
}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	const Keyboard& keyboard = m_inputManager->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::D))
	{
		ModelBase& model1 = *cubeBundle1.GetModel(0u);

		model1.GetTransform().MoveTowardsX(0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::A))
	{
		ModelBase& model1 = *cubeBundle1.GetModel(0u);

		model1.GetTransform().MoveTowardsX(-0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::C))
	{
		if (cubeBundleIndex2 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle2.AddModel(0.3f).AddModel(0.3f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle2.GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(1u);
			}

			{
				ModelBase& model2 = *cubeBundle2.GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Monika"));
				model2.SetDiffuseIndex(atlasBindingIndex);

				model2.GetTransform().MoveTowardsX(-1.5f);
			}

			cubeBundle2.SetMeshBundleIndex(cubeMeshBundleIndex);
			cubeBundleIndex2 = cubeBundle2.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex2);

			cubeBundleIndex2 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle2      = ModelBundleBase{};
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::V))
	{
		if (cubeBundleIndex3 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle3.AddModel(0.3f).AddModel(0.3f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle3.GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(whiteMatIndex);
				model1.SetDiffuseUVInfo(atlas.GetUVInfo("Unicorn"));
				model1.SetDiffuseIndex(atlasBindingIndex);

				model1.GetTransform().MoveTowardsX(-2.4f).MoveTowardsY(-1.2f);
			}

			{
				ModelBase& model2 = *cubeBundle3.GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Panda"));
				model2.SetDiffuseIndex(atlasBindingIndex);

				model2.GetTransform().MoveTowardsX(-0.8f).MoveTowardsY(-1.2f);
			}

			cubeBundle3.SetMeshBundleIndex(cubeMeshBundleIndex);
			cubeBundleIndex3 = cubeBundle3.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (cubeBundleIndex3 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex3);

			cubeBundleIndex3 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle3      = ModelBundleBase{};
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::N))
	{
		if (cubeBundleIndex4 == std::numeric_limits<std::uint32_t>::max())
		{
			cubeBundle4.AddModel(0.3f).AddModel(0.3f);

			// We have only a single mesh in the bundle.
			{
				ModelBase& model1 = *cubeBundle4.GetModel(0u);

				model1.SetMeshIndex(0u);
				model1.SetMaterialIndex(whiteMatIndex);
				model1.SetDiffuseUVInfo(atlas.GetUVInfo("UltraMarine"));
				model1.SetDiffuseIndex(atlasBindingIndex);

				model1.GetTransform().MoveTowardsX(2.4f).MoveTowardsY(-1.2f);
			}

			{
				ModelBase& model2 = *cubeBundle4.GetModel(1u);

				model2.SetMeshIndex(0u);
				model2.SetMaterialIndex(whiteMatIndex);
				model2.SetDiffuseUVInfo(atlas.GetUVInfo("Goku"));
				model2.SetDiffuseIndex(atlasBindingIndex);

				model2.GetTransform().MoveTowardsX(0.8f).MoveTowardsY(-1.2f);
			}

			cubeBundle4.SetMeshBundleIndex(cubeMeshBundleIndex);
			cubeBundleIndex4 = cubeBundle4.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (cubeBundleIndex4 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex4);

			cubeBundleIndex4 = std::numeric_limits<std::uint32_t>::max();
			cubeBundle4      = ModelBundleBase{};
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

		const size_t materialIndex = m_renderer->AddMaterial(tealMat);

		sTeal = std::move(tealMat);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Q))
	{
		if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
		{
			std::uint32_t index = 2u;

			if (sTeal)
				index = 3u;

			ModelBase& model1 = *cubeBundle2.GetModel(0u);

			model1.SetMaterialIndex(index);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::R))
	{
		sTeal.reset();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::O))
	{
		if (sphereMeshBundleIndex == std::numeric_limits<std::uint32_t>::max())
		{
			MeshBundleBase sphereMesh{};

			Mesh mesh{};

			SphereMesh{ 64u, 64u }.SetMesh(mesh);

			if (m_engineType == RenderEngineType::IndirectDraw)
				mesh.bounds = RectangleBounds{}.GetBounds(mesh.vertices);

			sphereMesh.AddMesh(std::move(mesh));

			sphereMeshBundleIndex = MeshBundleBase::SetMeshBundle(*m_renderer, std::move(sphereMesh));
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::P))
	{
		const bool bundle1Exists = cubeBundleIndex1 != std::numeric_limits<std::uint32_t>::max();
		const bool sphereExists  = sphereMeshBundleIndex != std::numeric_limits<std::uint32_t>::max();

		if (bundle1Exists && sphereExists)
		{
			cubeBundle1.GetModel(0u)->SetMeshIndex(0u);
			cubeBundle1.GetModel(1u)->SetMeshIndex(0u);

			cubeBundle1.SetMeshBundleIndex(sphereMeshBundleIndex);
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
			ModelBase& model1 = *cubeBundle1.GetModel(0u);

			model1.SetDiffuseUVInfo(UVInfo{});
			model1.SetDiffuseIndex(secondTextureIndex);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Four))
	{
		m_renderer->UnbindTexture(0u);
	}
}
