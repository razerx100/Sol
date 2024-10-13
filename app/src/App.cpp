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

static std::shared_ptr<ModelBundleBaseVS> sCubeBundle{};
static std::shared_ptr<ModelBundleBaseMS> sCubeBundleMS{};
static std::uint32_t cubeBundleIndex1 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle2{};
static std::shared_ptr<ModelBundleBaseMS> sCubeBundleMS2{};
static std::uint32_t cubeBundleIndex2 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle3{};
static std::shared_ptr<ModelBundleBaseMS> sCubeBundleMS3{};
static std::uint32_t cubeBundleIndex3 = 0u;
static std::shared_ptr<ModelBundleBaseVS> sCubeBundle4{};
static std::shared_ptr<ModelBundleBaseMS> sCubeBundleMS4{};
static std::uint32_t cubeBundleIndex4 = 0u;

static std::shared_ptr<MaterialBase> sTeal{};
static std::uint32_t cubeIndexCount     = 0u;
static std::uint32_t sphereIndexCount   = 0u;
static std::uint32_t cubeMeshletCount   = 0u;
static std::uint32_t sphereMeshletCount = 0u;
static std::uint32_t sphereIndex        = 0u;
static std::uint32_t whiteMatIndex      = 0u;
static bool isSphereAdded               = false;
static size_t secondTextureIndex        = std::numeric_limits<size_t>::max();
static TextureAtlas atlas{};
static std::uint32_t atlasBindingIndex  = 0u;

void App::Init()
{
	if (m_engineType == RenderEngineType::MeshDraw)
	{
		auto cubeMesh = std::make_unique<MeshBundleBaseMS>();

		CubeMesh{}.SetMesh(cubeMesh->GetBase(), CubeUVMode::IndependentFaceTexture);

		cubeMesh->SetMeshlets();

		cubeMeshletCount = static_cast<std::uint32_t>(std::size(cubeMesh->GetMeshlets()));

		const std::uint32_t meshIndex = m_renderer->AddMeshBundle(std::move(cubeMesh));
	}
	else
	{
		auto cubeMesh = std::make_unique<MeshBundleBaseVS>();

		CubeMesh{}.SetMesh(cubeMesh->GetBase(), CubeUVMode::IndependentFaceTexture);

		cubeIndexCount = static_cast<std::uint32_t>(std::size(cubeMesh->GetIndices()));

		if (m_engineType == RenderEngineType::IndirectDraw)
			cubeMesh->GetBase().SetBounds(RectangleBounds{});

		const std::uint32_t meshIndex = m_renderer->AddMeshBundle(std::move(cubeMesh));
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

	if (m_engineType == RenderEngineType::MeshDraw)
	{
		auto cubeBundle = std::make_shared<ModelBundleBaseMS>();

		{
			auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

			cubeBundle->AddModel(cube);

			cube->SetMeshDetailsMS(
				MeshDetailsMS{
					.meshletCount  = cubeMeshletCount,
					.meshletOffset = 0u
				}
			);

			cube->GetBase().SetMaterialIndex(whiteMatIndex);
			cube->GetBase().SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
			cube->GetBase().SetDiffuseIndex(atlasBindingIndex);
		}

		{
			auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

			cubeBundle->AddModel(cube);

			cube->SetMeshDetailsMS(
				MeshDetailsMS{
					.meshletCount  = cubeMeshletCount,
					.meshletOffset = 0u
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

		sCubeBundleMS = cubeBundle;
	}
	else
	{
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
}

void App::PhysicsUpdate()
{
	constexpr float cameraMoveSpeed = 0.5f;

	const Keyboard& keyboard = m_inputManager->GetKeyboard();

	if (keyboard.IsKeyPressed(SKeyCodes::D))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			auto& cube = sCubeBundleMS->GetModel(0u);
			cube->GetBase().GetTransform().MoveTowardsX(0.01f);
		}
		else
		{
			auto& cube = sCubeBundle->GetModel(0u);
			cube->GetBase().GetTransform().MoveTowardsX(0.01f);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::A))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			auto& cube = sCubeBundleMS->GetModel(0u);
			cube->GetBase().GetTransform().MoveTowardsX(-0.01f);
		}
		else
		{
			auto& cube = sCubeBundle->GetModel(0u);
			cube->GetBase().GetTransform().MoveTowardsX(-0.01f);
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::C))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (!sCubeBundleMS2)
			{
				auto cubeBundle = std::make_shared<ModelBundleBaseMS>();
				{
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
						}
					);

					cube->GetBase().SetMaterialIndex(1u);

					cubeBundle->AddModel(std::move(cube));
				}
				{
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
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

				sCubeBundleMS2 = std::move(cubeBundle);
			}
		}
		else
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
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Z))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (sCubeBundleMS2)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex2);

				sCubeBundleMS2.reset();
			}
		}
		else
		{
			if (sCubeBundle2)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex2);

				sCubeBundle2.reset();
			}
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::V))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (!sCubeBundleMS3)
			{
				auto cubeBundle = std::make_shared<ModelBundleBaseMS>();
				{
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
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
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
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

				sCubeBundleMS3 = std::move(cubeBundle);
			}
		}
		else
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
	}

	if (keyboard.IsKeyPressed(SKeyCodes::B))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (sCubeBundleMS3)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex3);

				sCubeBundleMS3.reset();
			}
		}
		else
		{
			if (sCubeBundle3)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex3);

				sCubeBundle3.reset();
			}
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::N))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (!sCubeBundleMS4)
			{
				auto cubeBundle = std::make_shared<ModelBundleBaseMS>();
				{
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
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
					auto cube = std::make_shared<ModelBaseMS>(ModelBase{ 0.3f });

					cube->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = cubeMeshletCount,
							.meshletOffset = 0u
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

				sCubeBundleMS4 = std::move(cubeBundle);
			}
		}
		else
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
	}

	if (keyboard.IsKeyPressed(SKeyCodes::M))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (sCubeBundleMS4)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex4);

				sCubeBundleMS4.reset();
			}
		}
		else
		{
			if (sCubeBundle4)
			{
				m_renderer->RemoveModelBundle(cubeBundleIndex4);

				sCubeBundle4.reset();
			}
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
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (sCubeBundleMS2)
			{
				std::uint32_t index = 2u;
				if (sTeal)
					index = 3u;

				auto& cube = sCubeBundleMS2->GetModel(0u);

				cube->GetBase().SetMaterialIndex(index);
			}
		}
		else
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
	}

	if (keyboard.IsKeyPressed(SKeyCodes::R))
	{
		sTeal.reset();
	}

	if (keyboard.IsKeyPressed(SKeyCodes::O))
	{
		if (!isSphereAdded)
		{
			if (m_engineType == RenderEngineType::MeshDraw)
			{
				auto sphereMesh = std::make_unique<MeshBundleBaseMS>();
				SphereMesh{ 64u, 64u }.SetMesh(sphereMesh->GetBase());

				sphereMesh->SetMeshlets();

				sphereMeshletCount = static_cast<std::uint32_t>(std::size(sphereMesh->GetMeshlets()));

				sphereIndex = m_renderer->AddMeshBundle(std::move(sphereMesh));
			}
			else
			{
				auto sphereMesh = std::make_unique<MeshBundleBaseVS>();
				SphereMesh{ 64u, 64u }.SetMesh(sphereMesh->GetBase());

				sphereIndexCount = static_cast<std::uint32_t>(std::size(sphereMesh->GetIndices()));

				if (m_engineType == RenderEngineType::IndirectDraw)
					sphereMesh->GetBase().SetBounds(RectangleBounds{});

				sphereIndex = m_renderer->AddMeshBundle(std::move(sphereMesh));
			}

			isSphereAdded = true;
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::P))
	{
		if (m_engineType == RenderEngineType::MeshDraw)
		{
			if (sCubeBundleMS && isSphereAdded)
			{
				auto& models = sCubeBundleMS->GetModels();

				for (auto& model : models)
					model->SetMeshDetailsMS(
						MeshDetailsMS{
							.meshletCount  = sphereMeshletCount,
							.meshletOffset = 0u
						}
					);

				sCubeBundleMS->SetMeshIndex(sphereIndex);
			}
		}
		else
		{
			if (sCubeBundle && isSphereAdded)
			{
				auto& models = sCubeBundle->GetModels();

				for (auto& model : models)
					model->SetMeshDetailsVS(
						MeshDetailsVS{
							.indexCount  = sphereIndexCount,
							.indexOffset = 0u
						}
					);

				sCubeBundle->SetMeshIndex(sphereIndex);
			}
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
			if (m_engineType == RenderEngineType::MeshDraw)
			{
				auto& cube = sCubeBundleMS->GetModel(0u);
				cube->GetBase().SetDiffuseUVInfo(UVInfo{});
				cube->GetBase().SetDiffuseIndex(secondTextureIndex);
			}
			else
			{
				auto& cube = sCubeBundle->GetModel(0u);
				cube->GetBase().SetDiffuseUVInfo(UVInfo{});
				cube->GetBase().SetDiffuseIndex(secondTextureIndex);
			}
		}
	}

	if (keyboard.IsKeyPressed(SKeyCodes::Four))
	{
		m_renderer->UnbindTexture(0u);
	}
}
