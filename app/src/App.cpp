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

// These can't be global variables, as then they will
// be in the Vertex mode always.
static MeshBundleGeneral& GetCubeMesh() noexcept
{
	static MeshBundleGeneral cubeMesh{};

	return cubeMesh;
}
static MeshBundleGeneral& GetSphereMesh() noexcept
{
	static MeshBundleGeneral sphereMesh{};

	return sphereMesh;
}

static std::uint32_t cubeMeshIndex   = std::numeric_limits<std::uint32_t>::max();;
static std::uint32_t sphereMeshIndex = std::numeric_limits<std::uint32_t>::max();;

// These can't be global variables, as then they will
// be in the Vertex mode always.
static ModelBundle& GetModelBundle1() noexcept
{
	static ModelBundle cubeBundle1{};

	return cubeBundle1;
}

static ModelBundle& GetModelBundle2() noexcept
{
	static ModelBundle cubeBundle2{};

	return cubeBundle2;
}

static ModelBundle& GetModelBundle3() noexcept
{
	static ModelBundle cubeBundle3{};

	return cubeBundle3;
}

static ModelBundle& GetModelBundle4() noexcept
{
	static ModelBundle cubeBundle4{};

	return cubeBundle4;
}

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
		Mesh mesh{};

		CubeMesh{}.SetMesh(mesh, CubeUVMode::IndependentFaceTexture);

		if (m_engineType == RenderEngineType::IndirectDraw)
			mesh.bounds = RectangleBounds{}.GetBounds(mesh.vertices);

		MeshBundleGeneral& cubeMesh = GetCubeMesh();

		cubeMesh.AddMesh(std::move(mesh));

		cubeMeshIndex = cubeMesh.SetMeshBundle(*m_renderer);
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
		ModelBundle& cubeBundle = GetModelBundle1();

		cubeBundle.AddModel(0.3f).AddModel(0.3f);

		const MeshDetails cubeMeshDetails = GetCubeMesh().GetMeshDetails(
			CubeMesh::GetName(CubeUVMode::IndependentFaceTexture)
		);

		cubeBundle.SetMeshModelDetails(0u, cubeMeshDetails);
		cubeBundle.SetMeshModelDetails(1u, cubeMeshDetails);

		{
			ModelBase& modelBase = cubeBundle.GetModel(0u);

			modelBase.SetMaterialIndex(whiteMatIndex);
			modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
			modelBase.SetDiffuseIndex(atlasBindingIndex);
		}

		{
			ModelBase& modelBase = cubeBundle.GetModel(1u);

			modelBase.SetMaterialIndex(whiteMatIndex);
			modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Katarin"));
			modelBase.SetDiffuseIndex(atlasBindingIndex);

			modelBase.GetTransform().MoveTowardsY(1.2f);
		}

		cubeBundleIndex1 = cubeBundle.SetModelBundle(*m_renderer, L"TestFragmentShader");
	}
}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	const Keyboard& keyboard = m_inputManager->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::D))
	{
		ModelBase& modelBase = GetModelBundle1().GetModel(0u);

		modelBase.GetTransform().MoveTowardsX(0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::A))
	{
		ModelBase& modelBase = GetModelBundle1().GetModel(0u);

		modelBase.GetTransform().MoveTowardsX(-0.01f);
	}

	if (keyboard.IsKeyPressed(SKeyCodes::C))
	{
		if (cubeBundleIndex2 == std::numeric_limits<std::uint32_t>::max())
		{
			ModelBundle& cubeBundle2 = GetModelBundle2();

			cubeBundle2.AddModel(0.3f).AddModel(0.3f);

			const MeshDetails cubeMeshDetails = GetCubeMesh().GetMeshDetails(
				CubeMesh::GetName(CubeUVMode::IndependentFaceTexture)
			);

			cubeBundle2.SetMeshModelDetails(0u, cubeMeshDetails);
			cubeBundle2.SetMeshModelDetails(1u, cubeMeshDetails);

			{
				ModelBase& modelBase = cubeBundle2.GetModel(0u);

				modelBase.SetMaterialIndex(1u);
			}

			{
				ModelBase& modelBase = cubeBundle2.GetModel(1u);

				modelBase.SetMaterialIndex(whiteMatIndex);
				modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Monika"));
				modelBase.SetDiffuseIndex(atlasBindingIndex);

				modelBase.GetTransform().MoveTowardsX(-1.5f);
			}

			cubeBundleIndex2 = cubeBundle2.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex2);

			cubeBundleIndex2  = std::numeric_limits<std::uint32_t>::max();

			GetModelBundle2() = ModelBundle{};
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::V))
	{
		if (cubeBundleIndex3 == std::numeric_limits<std::uint32_t>::max())
		{
			ModelBundle& cubeBundle3 = GetModelBundle3();

			cubeBundle3.AddModel(0.3f).AddModel(0.3f);

			const MeshDetails cubeMeshDetails = GetCubeMesh().GetMeshDetails(
				CubeMesh::GetName(CubeUVMode::IndependentFaceTexture)
			);

			cubeBundle3.SetMeshModelDetails(0u, cubeMeshDetails);
			cubeBundle3.SetMeshModelDetails(1u, cubeMeshDetails);

			{
				ModelBase& modelBase = cubeBundle3.GetModel(0u);

				modelBase.SetMaterialIndex(whiteMatIndex);
				modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Unicorn"));
				modelBase.SetDiffuseIndex(atlasBindingIndex);

				modelBase.GetTransform().MoveTowardsX(-2.4f).MoveTowardsY(-1.2f);
			}

			{
				ModelBase& modelBase = cubeBundle3.GetModel(1u);

				modelBase.SetMaterialIndex(whiteMatIndex);
				modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Panda"));
				modelBase.SetDiffuseIndex(atlasBindingIndex);

				modelBase.GetTransform().MoveTowardsX(-0.8f).MoveTowardsY(-1.2f);
			}

			cubeBundleIndex3 = cubeBundle3.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (cubeBundleIndex3 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex3);

			cubeBundleIndex3  = std::numeric_limits<std::uint32_t>::max();

			GetModelBundle3() = ModelBundle{};
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::N))
	{
		if (cubeBundleIndex4 == std::numeric_limits<std::uint32_t>::max())
		{
			ModelBundle& cubeBundle4 = GetModelBundle4();

			cubeBundle4.AddModel(0.3f).AddModel(0.3f);

			const MeshDetails cubeMeshDetails = GetCubeMesh().GetMeshDetails(
				CubeMesh::GetName(CubeUVMode::IndependentFaceTexture)
			);

			cubeBundle4.SetMeshModelDetails(0u, cubeMeshDetails);
			cubeBundle4.SetMeshModelDetails(1u, cubeMeshDetails);

			{
				ModelBase& modelBase = cubeBundle4.GetModel(0u);

				modelBase.SetMaterialIndex(whiteMatIndex);
				modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("UltraMarine"));
				modelBase.SetDiffuseIndex(atlasBindingIndex);

				modelBase.GetTransform().MoveTowardsX(2.4f).MoveTowardsY(-1.2f);
			}

			{
				ModelBase& modelBase = cubeBundle4.GetModel(1u);

				modelBase.SetMaterialIndex(whiteMatIndex);
				modelBase.SetDiffuseUVInfo(atlas.GetUVInfo("Goku"));
				modelBase.SetDiffuseIndex(atlasBindingIndex);

				modelBase.GetTransform().MoveTowardsX(0.8f).MoveTowardsY(-1.2f);
			}

			cubeBundleIndex4 = cubeBundle4.SetModelBundle(*m_renderer, L"TestFragmentShader");
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (cubeBundleIndex4 != std::numeric_limits<std::uint32_t>::max())
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex4);

			cubeBundleIndex4  = std::numeric_limits<std::uint32_t>::max();

			GetModelBundle4() = ModelBundle{};
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

			ModelBase& modelBase = GetModelBundle2().GetModel(0u);

			modelBase.SetMaterialIndex(index);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::R))
	{
		sTeal.reset();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::O))
	{
		if (sphereMeshIndex == std::numeric_limits<std::uint32_t>::max())
		{
			Mesh mesh{};

			SphereMesh{ 64u, 64u }.SetMesh(mesh);

			if (m_engineType == RenderEngineType::IndirectDraw)
				mesh.bounds = RectangleBounds{}.GetBounds(mesh.vertices);

			MeshBundleGeneral& sphereMesh = GetSphereMesh();

			sphereMesh.AddMesh(std::move(mesh));

			sphereMeshIndex = sphereMesh.SetMeshBundle(*m_renderer);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::P))
	{
		const bool bundle1Exists = cubeBundleIndex1 != std::numeric_limits<std::uint32_t>::max();
		const bool sphereExists  = sphereMeshIndex != std::numeric_limits<std::uint32_t>::max();

		if (bundle1Exists && sphereExists)
		{
			const MeshDetails sphereMeshDetails = GetSphereMesh().GetMeshDetails(
				SphereMesh::GetName(64u, 64u)
			);

			for (size_t index = 0u; index < 2u; ++index)
				GetModelBundle1().SetMeshModelDetails(index, sphereMeshDetails);

			GetModelBundle1().SetMeshIndex(sphereMeshIndex);
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
			ModelBase& modelBase = GetModelBundle1().GetModel(0u);

			modelBase.SetDiffuseUVInfo(UVInfo{});
			modelBase.SetDiffuseIndex(secondTextureIndex);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Four))
	{
		m_renderer->UnbindTexture(0u);
	}
}
