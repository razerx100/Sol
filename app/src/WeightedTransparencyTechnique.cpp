#include <WeightedTransparencyTechnique.hpp>
#include <ExternalBindingIndices.hpp>

namespace Sol
{
ShaderName WeightedTransparencyTechnique::s_compositePassShaderName = L"WeightedTransparencyCompositeShader";

WeightedTransparencyTechnique::WeightedTransparencyTechnique()
	: GraphicsTechniqueExtensionBase{}, m_accumulationRenderTarget{},
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
		ExternalFormat::R16_FLOAT,
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

ExternalGraphicsPipeline WeightedTransparencyTechnique::GetCompositePassPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline compositePassPipeline
		= graphicsPipelineManager.GetTransparencyCombinePassSignature();

	compositePassPipeline.SetVertexShader(graphicsPipelineManager.GetNoTransformVertexShader());
	compositePassPipeline.SetFragmentShader(s_compositePassShaderName);

	return compositePassPipeline;
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
}
