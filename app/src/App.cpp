#include <TextureTools.hpp>

#include <App.hpp>
#include <MaterialBase.hpp>
#include <BasicModels.hpp>
#include <BasicMeshBundles.hpp>
#include <CameraManagerSol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <TextureAtlas.hpp>
#include <MeshBounds.hpp>

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
static TextureAtlas atlas{};
static std::uint32_t atlasBindingIndex = 0u;

void App::Init()
{
	auto cubeMesh = std::make_unique<MeshBundleBaseVS>();

	CubeMesh{}.SetMesh(cubeMesh->GetBase(), CubeUVMode::IndependentFaceTexture);

	auto camera   = std::make_shared<PerspectiveCameraEuler>();

	camera->SetProjectionMatrix(1920u, 1080u);
	camera->SetCameraPosition(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f });

	cubeIndexCount = static_cast<std::uint32_t>(std::size(cubeMesh->GetIndices()));

	if (m_engineType == RenderEngineType::IndirectDraw)
		cubeMesh->GetBase().SetBounds(RectangleBounds{});

	const std::uint32_t meshIndex   = m_renderer->AddMeshBundle(std::move(cubeMesh));

	const std::uint32_t cameraIndex = m_renderer->AddCamera(camera);

	m_renderer->SetCamera(cameraIndex);

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

		const size_t materialIndex = m_renderer->AddMaterial(std::move(greenMat));
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

			const size_t textureIndex = m_renderer->AddTexture(std::move(atlastTex));
			atlasBindingIndex         = m_renderer->BindTexture(textureIndex);
		}
	}

	{
		auto sphereMesh = std::make_unique<MeshBundleBaseMS>();

		CubeMesh{}.SetMesh(sphereMesh->GetBase(), CubeUVMode::IndependentFaceTexture);

		sphereMesh->SetMeshlets();
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

	cubeBundleIndex1 = m_renderer->AddModelBundle(
		cubeBundle->GetBundleImpl(), L"TestFragmentShader"
	);

	sCubeBundle = cubeBundle;
}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	IKeyboard& keyboard = m_inputManager->GetKeyboard();

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

			cubeBundleIndex2 = m_renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle2 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (sCubeBundle2)
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex2);

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

			cubeBundleIndex3 = m_renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle3 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (sCubeBundle3)
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex3);

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

			cubeBundleIndex4 = m_renderer->AddModelBundle(
				cubeBundle->GetBundleImpl(), L"TestFragmentShader"
			);

			sCubeBundle4 = std::move(cubeBundle);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (sCubeBundle4)
		{
			m_renderer->RemoveModelBundle(cubeBundleIndex4);

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

		const size_t materialIndex = m_renderer->AddMaterial(tealMat);

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
			auto sphereMesh = std::make_unique<MeshBundleBaseVS>();
			SphereMesh{ 64u, 64u }.SetMesh(sphereMesh->GetBase());

			sphereIndexCount = static_cast<std::uint32_t>(std::size(sphereMesh->GetIndices()));

			if (m_engineType == RenderEngineType::IndirectDraw)
				sphereMesh->GetBase().SetBounds(RectangleBounds{});

			const std::uint32_t sphereIndex = m_renderer->AddMeshBundle(std::move(sphereMesh));

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
				const size_t textureIndex = m_renderer->AddTexture(std::move(texture));

				secondTextureIndex = m_renderer->BindTexture(textureIndex);
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
		m_renderer->UnbindTexture(0u);
	}
}
