#ifndef APP_HPP_
#define APP_HPP_
#include <InputManager.hpp>
#include <ExtensionManager.hpp>
#include <RenderPassManager.hpp>

#include <ModelBase.hpp>
#include <ModelContainer.hpp>
#include <BasicMeshBundles.hpp>
#include <CameraManagerSol.hpp>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <TextureAtlas.hpp>
#include <SceneMaterialProcessor.hpp>
#include <AllocationLiterals.hpp>
#include <SolScene.hpp>
#include <RendererUtility.hpp>

namespace ExampleApp
{
using namespace Sol;

template<InputModule inputModule>
class App
{
	template<InputModule inputModule>
	struct KeycodeConditional { using type = Pluto::SKeyCodes; };

	template<InputModule inputModule>
	struct KeyboardConditional { using type = Pluto::Keyboard; };

	using SKeyCodes = typename KeycodeConditional<inputModule>::type;
	using Keyboard  = typename KeyboardConditional<inputModule>::type;

public:
	template<class ExtensionManager_t, class RenderPassManager_t>
	App(
		ExtensionManager_t& extensionManager, RenderPassManager_t& renderPassManager,
		std::uint32_t frameCount, std::shared_ptr<ModelContainer> modelContainerArg
	) : modelContainer{ std::move(modelContainerArg) }
	{
		extensionManager.SetBlinnPhongLight(frameCount);
		extensionManager.SetWeightedTransparency();

		renderPassManager.SetTransparencyPass(extensionManager.GetWeightedTransparencySP());
	}

	template<class Renderer_t, class ExtensionManager_t, class RenderPassManager_t>
	void Init(
		Renderer_t& renderer, ExtensionManager_t& extensionManager,
		RenderPassManager_t& renderPassManager
	) {
		auto* blinnPhong       = extensionManager.GetBlinnPhongLight();

		auto* transparencyPass = extensionManager.GetWeightedTransparency();

		{
			const ExternalGraphicsPipeline nonLightOpaquePipeline
				= blinnPhong->GetOpaqueLightSrcPipeline(
					renderPassManager.GetGraphicsPipelineManager()
				);

			nonLightPSOIndex = renderer.AddGraphicsPipeline(nonLightOpaquePipeline);

			const ExternalGraphicsPipeline lightOpaquePipeline
				= blinnPhong->GetOpaqueLightDstPipeline(
					renderPassManager.GetGraphicsPipelineManager()
				);

			lightPSOIndex = renderer.AddGraphicsPipeline(lightOpaquePipeline);

			renderPassManager.AddPipeline(nonLightPSOIndex);
			renderPassManager.AddPipeline(lightPSOIndex);

			const ExternalGraphicsPipeline lightTransparentPipeline
				= blinnPhong->GetTransparentLightDstPipeline(
					renderPassManager.GetGraphicsPipelineManager()
				);

			lightTransparentPSOIndex = renderer.AddGraphicsPipeline(lightTransparentPipeline);

			if (transparencyPass)
				transparencyPass->AddTransparentPipeline(lightTransparentPSOIndex);

			using ShaderType = PSOStorage::ShaderType;

			PSOStorage::SetPipelineIndex(ShaderType::OpaqueLight, lightPSOIndex);
			PSOStorage::SetPipelineIndex(ShaderType::TransparentLight, lightTransparentPSOIndex);
		}

		{
			MeshBundleTempCustom testMeshBundle{};

			Mesh mesh{};

			CubeMesh{}.SetMesh(mesh, CubeUVMode::IndependentFaceTexture);
			testMeshBundle.AddMesh(std::move(mesh));

			Mesh quadMesh{};

			QuadMesh{}.SetMesh(quadMesh);
			testMeshBundle.AddMesh(std::move(quadMesh));

			testMeshBundleIndex = renderer.AddMeshBundle(
				GetPipelineSpecificMeshBundle(testMeshBundle, renderPassManager)
			);
		}

		{
			MeshBundleTempAssimp assimpMeshBundle{};

			auto sceneProcessor = std::make_shared<SceneProcessor>(
				"resources/meshes/shiba/scene.gltf"
			);

			SceneMaterialProcessor materialProcessor{ sceneProcessor };
			materialProcessor.ProcessMeshAndMaterialData();

			materialProcessor.LoadBlinnPhongMaterials(renderer, *blinnPhong);
			//materialProcessor.LoadTextures(*m_renderer);
			materialProcessor.LoadTexturesAsAtlas(renderer);

			assimpMeshBundle.SetSceneProcessor(sceneProcessor);

			testScene.SetSceneNodes(*sceneProcessor);
			testScene.SetMeshMaterialDetails(*sceneProcessor, materialProcessor);

			assimpMeshBundleIndex = renderer.AddMeshBundle(
				GetPipelineSpecificMeshBundle(assimpMeshBundle, renderPassManager)
			);
		}

		{
			GLTFObject gltf{};

			gltf.LoadFromFile("resources/meshes/shiba/scene.gltf");

			MeshBundleTemporaryData meshData = SceneMeshProcessor1::GenerateTemporaryMeshData(
				gltf, true
			);
		}

		cubeLightBundle = std::make_unique<ModelBundleBase>();

		cubeLightBundle->SetModelContainer(modelContainer);

		const size_t lightModelLocalIndex = cubeLightBundle->AddModel(
			nonLightPSOIndex, 0.1f
		);

		constexpr BlinnPhongLightType lightType = BlinnPhongLightType::Spotlight;

		std::uint32_t lightIndex = blinnPhong->AddLight(
			renderer, LightSource{ cubeLightBundle->GetIndexInContainer(lightModelLocalIndex) },
			lightType
		);

		std::uint32_t lightIndex1 = blinnPhong->AddLight(
			renderer, LightSource{}, BlinnPhongLightType::Directional
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

				// These are done in Cosine, so calculation on the shaders is easier. As we
				// will have to compare the cuttoffs with Dot products.
				lightProperties.innerCutoff = std::cos(DirectX::XMConvertToRadians(12.5f));
				lightProperties.outerCutoff = std::cos(DirectX::XMConvertToRadians(17.5f));

				break;
			}
		}

		blinnPhong->SetProperties(lightIndex, lightProperties);

		blinnPhong->SetProperties(lightIndex1, lightProperties);
		blinnPhong->SetDirection(lightIndex1, DirectX::XMFLOAT3{ 1.f, 0.f, 0.f });
		blinnPhong->SetType(lightIndex1, BlinnPhongLightType::Directional);

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

			const size_t materialIndex = blinnPhong->AddMaterial(renderer, white);

			whiteMatIndex = static_cast<std::uint32_t>(materialIndex);
		}

		{
			atlas.AddTexture("Narrative",        "resources/textures/NTHead.jpg")
				.AddTexture("Panda",             "resources/textures/Panda.png")
				.AddTexture("Unicorn",           "resources/textures/unicorn.jpeg")
				.AddTexture("Katarin",           "resources/textures/Katarin.png")
				.AddTexture("Monika",            "resources/textures/MonikaGun.png")
				.AddTexture("UltraMarine",       "resources/textures/UltraMarine.jpg")
				.AddTexture("Goku",              "resources/textures/Goku.jpg")
				.AddTexture("Container",         "resources/textures/container.png")
				.AddTexture("ContainerSpec",     "resources/textures/container_specular.png")
				.AddTexture("TransparentWindow", "resources/textures/blending_transparent_window.png");

			atlas.CreateAtlas();

			STexture atlastTex = atlas.MoveTexture();

			const size_t textureIndex = renderer.AddTexture(std::move(atlastTex));
			atlasBindingIndex         = renderer.BindTexture(textureIndex);
		}

		{
			{
				Model& model1 = cubeLightBundle->GetModel(0u);

				model1.SetMeshIndex(0u);

				ModelMaterial& modelMaterial = model1.GetMaterial();

				modelMaterial.SetMaterialIndex(whiteMatIndex);
				modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Narrative"));
				modelMaterial.SetDiffuseIndex(atlasBindingIndex);
			}

			cubeLightBundle->SetMeshBundleIndex(testMeshBundleIndex);

			cubeLightBundleIndex = renderer.AddModelBundle(cubeLightBundle->GetModelBundle());

			renderPassManager.AddModelBundle(cubeLightBundleIndex, renderer);
		}

		{
			cubeBundle1 = std::make_unique<ModelBundleBase>();

			cubeBundle1->SetModelContainer(modelContainer);

			cubeBundle1->AddModel(lightPSOIndex, 0.45f);
			cubeBundle1->AddModel(lightPSOIndex, 0.45f);

			// We have only a single mesh in the bundle.
			{
				Model& model1 = cubeBundle1->GetModel(0u);

				model1.SetMeshIndex(0u);

				ModelMaterial& modelMaterial = model1.GetMaterial();

				modelMaterial.SetMaterialIndex(whiteMatIndex);
				modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Container"));
				modelMaterial.SetDiffuseIndex(atlasBindingIndex);
				modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("ContainerSpec"));
				modelMaterial.SetSpecularIndex(atlasBindingIndex);

				cubeBundle1->MoveTowardsX(0u, 0.75f);
			}

			{
				Model& model2 = cubeBundle1->GetModel(1u);

				model2.SetMeshIndex(0u);

				ModelMaterial& modelMaterial = model2.GetMaterial();

				modelMaterial.SetMaterialIndex(whiteMatIndex);
				modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Katarin"));
				modelMaterial.SetDiffuseIndex(atlasBindingIndex);
				modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("Katarin"));
				modelMaterial.SetSpecularIndex(atlasBindingIndex);

				model2.SetVisibility(false);

				cubeBundle1->MoveTowardsY(1u, 0.75f);
			}

			cubeBundle1->SetMeshBundleIndex(testMeshBundleIndex);

			cubeBundleIndex1 = renderer.AddModelBundle(cubeBundle1->GetModelBundle());

			renderPassManager.AddModelBundle(cubeBundleIndex1, renderer);
		}

		{
			quadBundleT = std::make_unique<ModelBundleBase>();

			quadBundleT->SetModelContainer(modelContainer);

			quadBundleT->AddModel(lightTransparentPSOIndex, 0.45f);
			quadBundleT->AddModel(lightTransparentPSOIndex, 0.45f);

			// We have only a single mesh in the bundle.
			{
				Model& model1 = quadBundleT->GetModel(0u);

				model1.SetMeshIndex(1u);

				ModelMaterial& modelMaterial = model1.GetMaterial();

				modelMaterial.SetMaterialIndex(whiteMatIndex);
				modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("TransparentWindow"));
				modelMaterial.SetDiffuseIndex(atlasBindingIndex);
				modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("TransparentWindow"));
				modelMaterial.SetSpecularIndex(atlasBindingIndex);

				quadBundleT->MoveTowardsX(0u, 0.75f);
			}

			{
				Model& model2 = quadBundleT->GetModel(1u);

				model2.SetMeshIndex(1u);

				ModelMaterial& modelMaterial = model2.GetMaterial();

				modelMaterial.SetMaterialIndex(whiteMatIndex);
				modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("TransparentWindow"));
				modelMaterial.SetDiffuseIndex(atlasBindingIndex);
				modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("TransparentWindow"));
				modelMaterial.SetSpecularIndex(atlasBindingIndex);

				quadBundleT->MoveTowardsX(1u, 1.0f);
			}

			quadBundleT->SetMeshBundleIndex(testMeshBundleIndex);

			quadBundleIndexT = renderer.AddModelBundle(quadBundleT->GetModelBundle());

			if (transparencyPass)
				transparencyPass->AddTransparentModelBundle(quadBundleIndexT, renderer);
		}

	{
		assimpModelBundle1 = std::make_unique<ModelBundleBase>();

		assimpModelBundle1->SetModelContainer(modelContainer);

		assimpModelBundle1->SetMeshBundle(assimpMeshBundleIndex, 0.5f, testScene);
		assimpModelBundle1->MoveTowardsZ(0u, -1.f);
		//assimpModelBundle1->MoveTowardsY(0u, -0.75f);
		assimpModelBundle1->RotateRollDegree(0u, 90.f);
		assimpModelBundle1->RotatePitchDegree(0u, 90.f);
		assimpModelBundle1->SetMeshBundleIndex(assimpMeshBundleIndex);

		assimpBundleIndex1 = renderer.AddModelBundle(assimpModelBundle1->GetModelBundle());

		renderPassManager.AddModelBundle(assimpBundleIndex1, renderer);

		if (transparencyPass)
			transparencyPass->AddTransparentModelBundle(assimpBundleIndex1, renderer);
	}

		/*
		{
			assimpModelBundle2 = std::make_unique<ModelBundleBase>();

			assimpModelBundle2->SetMeshBundle(assimpMeshBundleIndex, 0.5f, assimpMeshBundle);
			assimpModelBundle2->MoveTowardsZ(0u, 1.f).MoveTowardsX(0u, 10.f);
			assimpModelBundle2->MoveTowardsY(0u, -0.75f);
			assimpModelBundle2->RotateRollDegree(0u, 90.f);
			assimpModelBundle2->RotatePitchDegree(0u, 90.f);
			assimpModelBundle2->SetMeshBundleIndex(assimpMeshBundleIndex);

			assimpBundleIndex2 = renderer.AddModelBundle(assimpModelBundle2->GetModelBundle());

			renderPassManager.AddModelBundle(assimpBundleIndex2, renderer);

			if (transparencyPass)
				transparencyPass->AddTransparentModelBundle(assimpBundleIndex2, renderer);
		}

		{
			assimpModelBundle3 = std::make_unique<ModelBundleBase>();

			assimpModelBundle3->SetMeshBundle(assimpMeshBundleIndex, 0.5f, assimpMeshBundle);
			assimpModelBundle3->MoveTowardsZ(0u, 1.f).MoveTowardsX(0u, -10.f);
			assimpModelBundle3->MoveTowardsY(0u, -0.75f);
			assimpModelBundle3->RotateRollDegree(0u, 90.f);
			assimpModelBundle3->RotatePitchDegree(0u, 90.f);
			assimpModelBundle3->SetMeshBundleIndex(assimpMeshBundleIndex);

			assimpBundleIndex3 = renderer.AddModelBundle(assimpModelBundle3->GetModelBundle());

			renderPassManager.AddModelBundle(assimpBundleIndex3, renderer);

			if (transparencyPass)
				transparencyPass->AddTransparentModelBundle(assimpBundleIndex3, renderer);
		}
		*/
	}

	template<
		class InputManager_t,
		class Renderer_t,
		class ExtensionManager_t,
		class RenderPassManager_t
	>
	void PhysicsUpdate(
		InputManager_t& inputManager, Renderer_t& renderer, ExtensionManager_t& extensionManager,
		RenderPassManager_t& renderPassManager, CameraManager& cameraManager
	) {
		const Keyboard& keyboard = inputManager.GetKeyboard();

		if (keyboard.IsKeyPressed(SKeyCodes::UpArrow))
			cubeLightBundle->MoveTowardsZ(0u, 0.05f);

		if (keyboard.IsKeyPressed(SKeyCodes::DownArrow))
			cubeLightBundle->MoveTowardsZ(0u, -0.05f);

		if (keyboard.IsKeyPressed(SKeyCodes::RightArrow))
			cubeLightBundle->MoveTowardsX(0u, 0.05f);

		if (keyboard.IsKeyPressed(SKeyCodes::LeftArrow))
			cubeLightBundle->MoveTowardsX(0u, -0.05f);

		if (keyboard.IsKeyPressed(SKeyCodes::W))
		{
			PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(
				renderPassManager.GetMainPassCameraIndex()
			);

			camera.MoveForward(0.01f);
		}

		if (keyboard.IsKeyPressed(SKeyCodes::S))
		{
			PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(
				renderPassManager.GetMainPassCameraIndex()
			);

			camera.MoveBackward(0.01f);
		}

		if (keyboard.IsKeyPressed(SKeyCodes::E))
		{
			PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(
				renderPassManager.GetMainPassCameraIndex()
			);

			camera.LookAround(0.01f, 0.f);
		}

		if (keyboard.IsKeyPressed(SKeyCodes::Q))
		{
			PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(
				renderPassManager.GetMainPassCameraIndex()
			);

			camera.LookAround(-0.01f, 0.f);
		}

		if (keyboard.IsKeyPressed(SKeyCodes::D))
			cubeBundle1->MoveTowardsX(0u, 0.01f);

		if (keyboard.IsKeyPressed(SKeyCodes::A))
			cubeBundle1->MoveTowardsX(0u, -0.01f);

		if (keyboard.IsKeyPressed(SKeyCodes::C))
		{
			if (cubeBundleIndex2 == std::numeric_limits<std::uint32_t>::max())
			{
				cubeBundle2 = std::make_unique<ModelBundleBase>();

				cubeBundle2->SetModelContainer(modelContainer);

				cubeBundle2->AddModel(lightPSOIndex, 0.45f);
				cubeBundle2->AddModel(lightPSOIndex, 0.45f);

				// We have only a single mesh in the bundle.
				{
					Model& model1 = cubeBundle2->GetModel(0u);

					model1.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model1.GetMaterial();

					modelMaterial.SetMaterialIndex(1u);
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularIndex(atlasBindingIndex);
				}

				{
					Model& model2 = cubeBundle2->GetModel(1u);

					model2.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model2.GetMaterial();

					modelMaterial.SetMaterialIndex(whiteMatIndex);
					modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Monika"));
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("Monika"));
					modelMaterial.SetSpecularIndex(atlasBindingIndex);

					cubeBundle2->MoveTowardsX(1u, -0.75f);
				}

				cubeBundle2->SetMeshBundleIndex(testMeshBundleIndex);

				cubeBundleIndex2 = renderer.AddModelBundle(cubeBundle2->GetModelBundle());

				renderPassManager.AddModelBundle(cubeBundleIndex2, renderer);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::Z))
		{
			if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
			{
				RendererUtility::RemoveModelBundle(renderer, modelContainer, cubeBundleIndex2);
				renderPassManager.RemoveModelBundle(cubeBundleIndex2);

				cubeBundleIndex2 = std::numeric_limits<std::uint32_t>::max();
				cubeBundle2.reset();
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::V))
		{
			if (cubeBundleIndex3 == std::numeric_limits<std::uint32_t>::max())
			{
				cubeBundle3 = std::make_unique<ModelBundleBase>();

				cubeBundle3->SetModelContainer(modelContainer);

				cubeBundle3->AddModel(lightPSOIndex, 0.45f);
				cubeBundle3->AddModel(lightPSOIndex, 0.45f);

				// We have only a single mesh in the bundle.
				{
					Model& model1 = cubeBundle3->GetModel(0u);

					model1.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model1.GetMaterial();

					modelMaterial.SetMaterialIndex(whiteMatIndex);
					modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Unicorn"));
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("Unicorn"));
					modelMaterial.SetSpecularIndex(atlasBindingIndex);

					cubeBundle3->MoveModel(0u, DirectX::XMFLOAT3{ -1.4f, -0.75f, 0.f });
				}

				{
					Model& model2 = cubeBundle3->GetModel(1u);

					model2.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model2.GetMaterial();

					modelMaterial.SetMaterialIndex(whiteMatIndex);
					modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Panda"));
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("Panda"));
					modelMaterial.SetSpecularIndex(atlasBindingIndex);

					cubeBundle3->MoveModel(1u, DirectX::XMFLOAT3{ -0.45f, -0.75f, 0.f });
				}

				cubeBundle3->SetMeshBundleIndex(testMeshBundleIndex);

				cubeBundleIndex3 = renderer.AddModelBundle(cubeBundle3->GetModelBundle());

				renderPassManager.AddModelBundle(cubeBundleIndex3, renderer);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::B))
		{
			if (cubeBundleIndex3 != std::numeric_limits<std::uint32_t>::max())
			{
				RendererUtility::RemoveModelBundle(renderer, modelContainer, cubeBundleIndex3);
				renderPassManager.RemoveModelBundle(cubeBundleIndex3);

				cubeBundleIndex3 = std::numeric_limits<std::uint32_t>::max();
				cubeBundle3.reset();
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::N))
		{
			if (cubeBundleIndex4 == std::numeric_limits<std::uint32_t>::max())
			{
				cubeBundle4 = std::make_unique<ModelBundleBase>();

				cubeBundle4->SetModelContainer(modelContainer);

				cubeBundle4->AddModel(lightPSOIndex, 0.45f);
				cubeBundle4->AddModel(lightPSOIndex, 0.45f);

				// We have only a single mesh in the bundle.
				{
					Model& model1 = cubeBundle4->GetModel(0u);

					model1.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model1.GetMaterial();

					modelMaterial.SetMaterialIndex(whiteMatIndex);
					modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("UltraMarine"));
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("UltraMarine"));
					modelMaterial.SetSpecularIndex(atlasBindingIndex);

					cubeBundle4->MoveModel(0u, DirectX::XMFLOAT3{ 1.4f, -0.75f, 0.f });
				}

				{
					Model& model2 = cubeBundle4->GetModel(1u);

					model2.SetMeshIndex(0u);

					ModelMaterial& modelMaterial = model2.GetMaterial();

					modelMaterial.SetMaterialIndex(whiteMatIndex);
					modelMaterial.SetDiffuseUVInfo(atlas.GetUVInfo("Goku"));
					modelMaterial.SetDiffuseIndex(atlasBindingIndex);
					modelMaterial.SetSpecularUVInfo(atlas.GetUVInfo("Goku"));
					modelMaterial.SetSpecularIndex(atlasBindingIndex);

					cubeBundle4->MoveModel(1u, DirectX::XMFLOAT3{ 0.45f, -0.75f, 0.f });
				}

				cubeBundle4->SetMeshBundleIndex(testMeshBundleIndex);

				cubeBundleIndex4 = renderer.AddModelBundle(cubeBundle4->GetModelBundle());

				renderPassManager.AddModelBundle(cubeBundleIndex4, renderer);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::M))
		{
			if (cubeBundleIndex4 != std::numeric_limits<std::uint32_t>::max())
			{
				RendererUtility::RemoveModelBundle(renderer, modelContainer, cubeBundleIndex4);
				renderPassManager.RemoveModelBundle(cubeBundleIndex4);

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

				auto* blinnPhong = extensionManager.GetBlinnPhongLight();

				tealMatIndex = blinnPhong->AddMaterial(renderer, teal);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::G))
		{
			if (cubeBundleIndex2 != std::numeric_limits<std::uint32_t>::max())
			{
				std::uint32_t index = whiteMatIndex;

				if (tealMatIndex != std::numeric_limits<std::uint32_t>::max())
					index = tealMatIndex;

				Model& model1 = cubeBundle2->GetModel(0u);

				model1.GetMaterial().SetMaterialIndex(index);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::R))
		{
			if (tealMatIndex != std::numeric_limits<std::uint32_t>::max())
			{
				auto* blinnPhong = extensionManager.GetBlinnPhongLight();

				blinnPhong->RemoveMaterial(tealMatIndex);

				tealMatIndex = std::numeric_limits<std::uint32_t>::max();
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::O))
		{
			if (sphereMeshBundleIndex == std::numeric_limits<std::uint32_t>::max())
			{
				MeshBundleTempCustom sphereMeshBundle{};

				Mesh mesh{};

				SphereMesh{ 64u, 64u }.SetMesh(mesh);
				sphereMeshBundle.AddMesh(std::move(mesh));

				sphereMeshBundleIndex = renderer.AddMeshBundle(
					GetPipelineSpecificMeshBundle(sphereMeshBundle, renderPassManager)
				);
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::P))
		{
			const bool bundle1Exists
				= cubeBundleIndex1 != std::numeric_limits<std::uint32_t>::max();
			const bool sphereExists
				= sphereMeshBundleIndex != std::numeric_limits<std::uint32_t>::max();

			if (bundle1Exists && sphereExists)
			{
				Model& model1 = cubeBundle1->GetModel(0u);

				model1.SetMeshIndex(0u);

				ModelMaterial& modelMaterial = model1.GetMaterial();

				modelMaterial.SetDiffuseIndex(0u);
				modelMaterial.SetSpecularIndex(0u);
				modelMaterial.SetDiffuseUVInfo(UVInfo{});
				modelMaterial.SetSpecularUVInfo(UVInfo{});

				Model& model2 = cubeBundle1->GetModel(1u);

				model2.SetMeshIndex(0u);

				ModelMaterial& modelMaterial2 = model2.GetMaterial();

				modelMaterial2.SetDiffuseIndex(0u);
				modelMaterial2.SetSpecularIndex(0u);
				modelMaterial2.SetDiffuseUVInfo(UVInfo{});
				modelMaterial2.SetSpecularUVInfo(UVInfo{});

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
					const size_t textureIndex = renderer.AddTexture(std::move(texture));

					secondTextureIndex = renderer.BindTexture(textureIndex);
				}
			}
		}

		if (keyboard.IsKeyPressed(SKeyCodes::Three))
		{
			if (secondTextureIndex != std::numeric_limits<size_t>::max())
			{
				Model& model1 = cubeBundle1->GetModel(0u);

				ModelMaterial& modelMaterial = model1.GetMaterial();

				modelMaterial.SetDiffuseUVInfo(UVInfo{});
				modelMaterial.SetDiffuseIndex(secondTextureIndex);
			}
		}
	}

private:
	template<class MeshBundle_t, class RenderPassManager_t>
	[[nodiscard]]
	MeshBundleTemporaryData GetPipelineSpecificMeshBundle(
		MeshBundle_t& meshBundle, const RenderPassManager_t& renderPassManager
	) {
		const bool isMeshPipeline
			= renderPassManager.GetGraphicsPipelineManager().IsMeshShaderPipeline();

		return meshBundle.GenerateTemporaryData(isMeshPipeline);
	}

private:
	std::shared_ptr<ModelContainer> modelContainer{};
	SolScene testScene{};
	std::uint32_t testMeshBundleIndex   = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t assimpMeshBundleIndex = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t sphereMeshBundleIndex = std::numeric_limits<std::uint32_t>::max();

	std::unique_ptr<ModelBundleBase> cubeBundle1{};
	std::unique_ptr<ModelBundleBase> assimpModelBundle1{};
	std::unique_ptr<ModelBundleBase> assimpModelBundle2{};
	std::unique_ptr<ModelBundleBase> assimpModelBundle3{};
	std::unique_ptr<ModelBundleBase> cubeBundle2{};
	std::unique_ptr<ModelBundleBase> cubeBundle3{};
	std::unique_ptr<ModelBundleBase> cubeBundle4{};
	std::unique_ptr<ModelBundleBase> quadBundleT{};
	std::unique_ptr<ModelBundleBase> cubeLightBundle{};
	std::uint32_t cubeBundleIndex1     = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t assimpBundleIndex1   = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t assimpBundleIndex2   = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t assimpBundleIndex3   = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t cubeBundleIndex2     = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t cubeBundleIndex3     = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t cubeBundleIndex4     = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t quadBundleIndexT     = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t cubeLightBundleIndex = std::numeric_limits<std::uint32_t>::max();

	std::uint32_t whiteMatIndex      = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t tealMatIndex       = std::numeric_limits<std::uint32_t>::max();
	size_t secondTextureIndex        = std::numeric_limits<size_t>::max();
	TextureAtlas atlas{};
	std::uint32_t atlasBindingIndex  = std::numeric_limits<std::uint32_t>::max();

	std::uint32_t nonLightPSOIndex         = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t lightPSOIndex            = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t lightTransparentPSOIndex = std::numeric_limits<std::uint32_t>::max();

public:
	App(const App&) = delete;
	App& operator=(const App&) = delete;

	App(App&& other) noexcept
		: modelContainer{ std::move(other.modelContainer) },
		testScene{ std::move(other.testScene) },
		testMeshBundleIndex{ other.testMeshBundleIndex },
		assimpMeshBundleIndex{ other.assimpMeshBundleIndex },
		sphereMeshBundleIndex{ other.sphereMeshBundleIndex },

		cubeBundle1{ std::move(other.cubeBundle1) },
		assimpModelBundle1{ std::move(other.assimpModelBundle1) },
		assimpModelBundle2{ std::move(other.assimpModelBundle2) },
		assimpModelBundle3{ std::move(other.assimpModelBundle3) },
		cubeBundle2{ std::move(other.cubeBundle2) },
		cubeBundle3{ std::move(other.cubeBundle3) },
		cubeBundle4{ std::move(other.cubeBundle4) },
		quadBundleT{ std::move(other.quadBundleT) },
		cubeLightBundle{ std::move(other.cubeLightBundle) },
		cubeBundleIndex1{ other.cubeBundleIndex1 },
		assimpBundleIndex1{ other.assimpBundleIndex1 },
		assimpBundleIndex2{ other.assimpBundleIndex2 },
		assimpBundleIndex3{ other.assimpBundleIndex3 },
		cubeBundleIndex2{ other.cubeBundleIndex2 },
		cubeBundleIndex3{ other.cubeBundleIndex3 },
		cubeBundleIndex4{ other.cubeBundleIndex4 },
		quadBundleIndexT{ other.quadBundleIndexT },
		cubeLightBundleIndex{ other.cubeLightBundleIndex },

		whiteMatIndex{ other.whiteMatIndex },
		tealMatIndex{ other.tealMatIndex },
		secondTextureIndex{ other.secondTextureIndex },
		atlas{ std::move(other.atlas) },
		atlasBindingIndex{ other.atlasBindingIndex },

		nonLightPSOIndex{ other.nonLightPSOIndex },
		lightPSOIndex{ other.lightPSOIndex },
		lightTransparentPSOIndex{ other.lightTransparentPSOIndex }
	{}

	App& operator=(App&& other) noexcept
	{
		modelContainer        = std::move(other.modelContainer);
		testScene             = std::move(other.testScene);
		testMeshBundleIndex   = other.testMeshBundleIndex;
		assimpMeshBundleIndex = other.assimpMeshBundleIndex;
		sphereMeshBundleIndex = other.sphereMeshBundleIndex;

		cubeBundle1          = std::move(other.cubeBundle1);
		assimpModelBundle1   = std::move(other.assimpModelBundle1);
		assimpModelBundle2   = std::move(other.assimpModelBundle2);
		assimpModelBundle3   = std::move(other.assimpModelBundle3);
		cubeBundle2          = std::move(other.cubeBundle2);
		cubeBundle3          = std::move(other.cubeBundle3);
		cubeBundle4          = std::move(other.cubeBundle4);
		quadBundleT          = std::move(other.quadBundleT);
		cubeLightBundle      = std::move(other.cubeLightBundle);
		cubeBundleIndex1     = other.cubeBundleIndex1;
		assimpBundleIndex1   = other.assimpBundleIndex1;
		assimpBundleIndex2   = other.assimpBundleIndex2;
		assimpBundleIndex3   = other.assimpBundleIndex3;
		cubeBundleIndex2     = other.cubeBundleIndex2;
		cubeBundleIndex3     = other.cubeBundleIndex3;
		cubeBundleIndex4     = other.cubeBundleIndex4;
		quadBundleIndexT     = other.quadBundleIndexT;
		cubeLightBundleIndex = other.cubeLightBundleIndex;

		whiteMatIndex      = other.whiteMatIndex;
		tealMatIndex       = other.tealMatIndex;
		secondTextureIndex = other.secondTextureIndex;
		atlas              = std::move(other.atlas);
		atlasBindingIndex  = other.atlasBindingIndex;

		nonLightPSOIndex         = other.nonLightPSOIndex;
		lightPSOIndex            = other.lightPSOIndex;
		lightTransparentPSOIndex = other.lightTransparentPSOIndex;

		return *this;
	}
};
}
#endif
