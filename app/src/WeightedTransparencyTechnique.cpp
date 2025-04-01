#include <WeightedTransparencyTechnique.hpp>
#include <ExternalBindingIndices.hpp>

ShaderName WeightedTransparencyTechnique::s_compositePassShaderName = L"WeightedTransparencyCompositeShader";

WeightedTransparencyTechnique::WeightedTransparencyTechnique(Renderer* renderer)
	: GraphicsTechniqueExtensionBase{ renderer }, m_accumulationRenderTarget{},
	m_revealageRenderTarget{}, m_renderTargetBindingDataExtBuffer{}, m_transparencyPass{},
	m_postProcessingPass{}, m_accumulationTextureIndex{ 0u }, m_revealageTextureIndex{ 0u },
	m_bindingData{
		.accumulationRTBindingIndex = std::numeric_limits<std::uint32_t>::max(),
		.revealageRTBindingIndex    = std::numeric_limits<std::uint32_t>::max()
	}, m_accumulationBarrierIndex{ 0u }, m_revealageBarrierIndex{ 0u }
{
	constexpr size_t bufferBindingCount = 1u;

	m_bufferBindingDetails.resize(bufferBindingCount);

	m_bufferBindingDetails[s_rtBindingDataBufferIndex] = ExternalBufferBindingDetails
	{
		.layoutInfo =
		{
			.bindingIndex = ExternalBinding::s_transparencyRenderTargetBindingData,
			.type         = ExternalBufferType::CPUVisibleUniform
		}
	};
}

void WeightedTransparencyTechnique::SetFixedDescriptors()
{
	UpdateCPUBufferDescriptor(
		s_rtBindingDataBufferIndex, m_renderTargetBindingDataExtBuffer->BufferSize()
	);
}

void WeightedTransparencyTechnique::SetBuffers(ExternalResourceFactory* resourceFactory)
{
	constexpr size_t externalBufferCount = 1u;

	m_externalBufferIndices.resize(externalBufferCount);

	// Render Target Binding Data
	{
		const size_t bufferIndex    = s_rtBindingDataBufferIndex;
		const size_t extBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleUniform
		);

		m_renderTargetBindingDataExtBuffer = resourceFactory->GetExternalBufferSP(extBufferIndex);

		const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = extBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = extBufferIndexU32;

		m_renderTargetBindingDataExtBuffer->Create(sizeof(RenderTargetBindingData));
	}

	// Accumulation Render Target
	{
		const size_t accumulationTextureIndex = resourceFactory->CreateExternalTexture();

		m_accumulationTextureIndex = static_cast<std::uint32_t>(accumulationTextureIndex);

		m_accumulationRenderTarget = resourceFactory->GetExternalTextureSP(accumulationTextureIndex);
	}

	// Revealage Render Target
	{
		const size_t revealageTextureIndex = resourceFactory->CreateExternalTexture();

		m_revealageTextureIndex = static_cast<std::uint32_t>(revealageTextureIndex);

		m_revealageRenderTarget = resourceFactory->GetExternalTextureSP(revealageTextureIndex);
	}
}

void WeightedTransparencyTechnique::BindRenderTargetTextures()
{
	constexpr auto uint32Max    = std::numeric_limits<std::uint32_t>::max();

	const bool areTexturesBound = m_bindingData.accumulationRTBindingIndex != uint32Max
		&& m_bindingData.revealageRTBindingIndex != uint32Max;

	if (areTexturesBound)
	{
		m_renderer->RebindExternalTexture(
			m_accumulationTextureIndex, m_bindingData.accumulationRTBindingIndex
		);
		m_renderer->RebindExternalTexture(
			m_revealageTextureIndex, m_bindingData.revealageRTBindingIndex
		);
	}
	else
	{
		m_bindingData.accumulationRTBindingIndex = m_renderer->BindExternalTexture(
			m_accumulationTextureIndex
		);
		m_bindingData.revealageRTBindingIndex = m_renderer->BindExternalTexture(m_revealageTextureIndex);

		std::uint8_t* bindingDataCpuStart = m_renderTargetBindingDataExtBuffer->CPUHandle();

		memcpy(bindingDataCpuStart, &m_bindingData, sizeof(RenderTargetBindingData));
	}
}

void WeightedTransparencyTechnique::SetTransparencyPass(
	std::shared_ptr<ExternalRenderPass> transparencyPass
) noexcept {
	m_transparencyPass = std::move(transparencyPass);
}

void WeightedTransparencyTechnique::SetupTransparencyPass(std::uint32_t depthTextureIndex)
{
	const std::uint32_t accumTargetIndex = m_transparencyPass->AddRenderTarget(
		m_accumulationTextureIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_transparencyPass->SetRenderTargetClearColour(
		accumTargetIndex, DirectX::XMFLOAT4{ 0.f, 0.f, 0.f, 0.f }
	);

	const std::uint32_t revealTargetIndex = m_transparencyPass->AddRenderTarget(
		m_revealageTextureIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_transparencyPass->SetRenderTargetClearColour(
		revealTargetIndex, DirectX::XMFLOAT4{ 1.f, 0.f, 0.f, 0.f }
	);

	m_transparencyPass->SetDepthTesting(
		depthTextureIndex, ExternalAttachmentLoadOp::Load, ExternalAttachmentStoreOp::DontCare
	);
}

void WeightedTransparencyTechnique::SetupTransparencyGraphicsPipelineSignatures(
	GraphicsPipelineManager& graphicsPipelineManager, ExternalFormat depthFormat
) {
	ExternalGraphicsPipeline transparencyPassSignature{};

	transparencyPassSignature.EnableBackfaceCulling();

	transparencyPassSignature.AddRenderTarget(
		ExternalFormat::R16G16B16A16_FLOAT,
		ExternalBlendState
		{
			.enabled        = true,
			.alphaBlendOP   = ExternalBlendOP::Add,
			.colourBlendOP  = ExternalBlendOP::Add,
			.alphaBlendSrc  = ExternalBlendFactor::One,
			.alphaBlendDst  = ExternalBlendFactor::One,
			.colourBlendSrc = ExternalBlendFactor::One,
			.colourBlendDst = ExternalBlendFactor::One
		}
	);

	transparencyPassSignature.AddRenderTarget(
		ExternalFormat::R8_UNORM,
		ExternalBlendState
		{
			.enabled        = true,
			.alphaBlendOP   = ExternalBlendOP::Add,
			.colourBlendOP  = ExternalBlendOP::Add,
			.alphaBlendSrc  = ExternalBlendFactor::One,
			.alphaBlendDst  = ExternalBlendFactor::One,
			.colourBlendSrc = ExternalBlendFactor::Zero,
			.colourBlendDst = ExternalBlendFactor::OneMinusSrcColour
		}
	);

	transparencyPassSignature.EnableDepthTesting(depthFormat, false);

	graphicsPipelineManager.SetTransparencyPassSignature(std::move(transparencyPassSignature));
}

void WeightedTransparencyTechnique::SetupTransparencyCompositePipelineSignature(
	GraphicsPipelineManager& graphicsPipelineManager
) {
	ExternalGraphicsPipeline transparencyCombineSignature{};

	transparencyCombineSignature.AddRenderTarget(
		m_renderer->GetSwapchainFormat(),
		ExternalBlendState
		{
			.enabled        = true,
			.alphaBlendOP   = ExternalBlendOP::Add,
			.colourBlendOP  = ExternalBlendOP::Add,
			.alphaBlendSrc  = ExternalBlendFactor::SrcAlpha,
			.alphaBlendDst  = ExternalBlendFactor::OneMinusSrcAlpha,
			.colourBlendSrc = ExternalBlendFactor::One,
			.colourBlendDst = ExternalBlendFactor::One
		}
	);

	transparencyCombineSignature.DisableGPUCulling();

	graphicsPipelineManager.SetTransparencyCombinePassSignature(
		std::move(transparencyCombineSignature)
	);
}

void WeightedTransparencyTechnique::ResizeRenderTargets(std::uint32_t width, std::uint32_t height)
{
	m_accumulationRenderTarget->Create(
		width, height, ExternalFormat::R16G16B16A16_FLOAT,
		ExternalTexture2DType::RenderTarget, ExternalTextureCreationFlags{ .sampleTexture = true }
	);

	m_revealageRenderTarget->Create(
		width, height, ExternalFormat::R8_UNORM, ExternalTexture2DType::RenderTarget,
		ExternalTextureCreationFlags{ .sampleTexture = true }
	);

	m_transparencyPass->ResetAttachmentReferences();

	BindRenderTargetTextures();

	m_postProcessingPass->UpdateStartBarrierResource(
		m_accumulationBarrierIndex, m_accumulationTextureIndex
	);
	m_postProcessingPass->UpdateStartBarrierResource(
		m_revealageBarrierIndex, m_revealageTextureIndex
	);
}

ExternalGraphicsPipeline WeightedTransparencyTechnique::GetCompositePassPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline compositePassPipeline
		= graphicsPipelineManager.GetTransparencyCombinePassSignature();

	compositePassPipeline.SetVertexShader(graphicsPipelineManager.GetNoTransformVertexShader());
	compositePassPipeline.SetFragmentShader(s_compositePassShaderName);

	return compositePassPipeline;
}

void WeightedTransparencyTechnique::SetupCompositePassPipeline(
	ExternalRenderPass* postProcessingPass, const GraphicsPipelineManager& graphicsPipelineManager,
	ModelBundleBase& renderTargetQuadBundle, std::uint32_t renderTargetQuadMeshIndex
) {
	const std::uint32_t compositePipelineIndex = m_renderer->AddGraphicsPipeline(
		GetCompositePassPipeline(graphicsPipelineManager)
	);

	postProcessingPass->AddPipeline(compositePipelineIndex);

	auto quadModel = std::make_shared<ModelBase>();

	const auto& models = renderTargetQuadBundle.GetModels();

	// Copy the shared values if exists. As we just need a different instance with a different pipeline.
	if (!std::empty(models))
		quadModel->CopySharedValues(*models[0]);

	quadModel->SetMeshIndex(renderTargetQuadMeshIndex);
	quadModel->SetPipelineIndex(compositePipelineIndex);

	renderTargetQuadBundle.AddModel(std::move(quadModel));
}

void WeightedTransparencyTechnique::SetCompositePass(
	std::shared_ptr<ExternalRenderPass> postProcessingPass
) {
	m_accumulationBarrierIndex = postProcessingPass->AddStartBarrier(
		m_accumulationTextureIndex, ExternalTextureTransition::FragmentShaderReadOnly
	);

	m_revealageBarrierIndex = postProcessingPass->AddStartBarrier(
		m_revealageTextureIndex, ExternalTextureTransition::FragmentShaderReadOnly
	);

	m_postProcessingPass = std::move(postProcessingPass);
}
