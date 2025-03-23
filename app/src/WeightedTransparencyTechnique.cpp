#include <WeightedTransparencyTechnique.hpp>
#include <ExternalBindingIndices.hpp>

ShaderName WeightedTransparencyTechnique::s_compositePassShaderName = L"WeightedTransparencyCompositeShader";

WeightedTransparencyTechnique::WeightedTransparencyTechnique(Renderer* renderer)
	: GraphicsTechniqueExtensionBase{ renderer }, m_accumulationRenderTarget{},
	m_revealageRenderTarget{}, m_renderTargetBindingDataExtBuffer{}, m_transparencyPass{},
	m_accumulationTextureIndex{ 0u }, m_revealageTextureIndex{ 0u }
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
	RenderTargetBindingData bindingData
	{
		.accumulationRTBindingIndex = m_renderer->BindExternalTexture(m_accumulationTextureIndex),
		.revealageRTBindingIndex    = m_renderer->BindExternalTexture(m_revealageTextureIndex)
	};

	std::uint8_t* bindingDataCpuStart = m_renderTargetBindingDataExtBuffer->CPUHandle();

	memcpy(bindingDataCpuStart, &bindingData, sizeof(RenderTargetBindingData));
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

	graphicsPipelineManager.SetTransparencyCombinePassSignature(
		std::move(transparencyCombineSignature)
	);
}

void WeightedTransparencyTechnique::ResizeRenderTargets(std::uint32_t width, std::uint32_t height)
{
	m_accumulationRenderTarget->Create(
		width, height, ExternalFormat::R16G16B16A16_FLOAT,
		ExternalTexture2DType::RenderTarget, false, false
	);

	m_revealageRenderTarget->Create(
		width, height, ExternalFormat::R8_UNORM, ExternalTexture2DType::RenderTarget, false, false
	);

	m_transparencyPass->ResetAttachmentReferences();
}

ExternalGraphicsPipeline WeightedTransparencyTechnique::GetCompositePassPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline compositePassPipeline
		= graphicsPipelineManager.GetTransparencyCombinePassSignature();

	compositePassPipeline.SetFragmentShader(s_compositePassShaderName);

	return compositePassPipeline;
}

void WeightedTransparencyTechnique::SetupCompositePassPipeline(
	ExternalRenderPass* postProcessingPass, const GraphicsPipelineManager& graphicsPipelineManager
) {
	const std::uint32_t compositePipelineIndex = m_renderer->AddGraphicsPipeline(
		GetCompositePassPipeline(graphicsPipelineManager)
	);

	postProcessingPass->AddPipeline(compositePipelineIndex);
}
