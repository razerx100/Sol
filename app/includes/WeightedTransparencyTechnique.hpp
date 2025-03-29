#ifndef WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#define WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalResourceFactory.hpp>
#include <ExternalRenderPass.hpp>
#include <GraphicsPipelineManager.hpp>

class WeightedTransparencyTechnique : public GraphicsTechniqueExtensionBase
{
public:
	struct RenderTargetBindingData
	{
		std::uint32_t accumulationRTBindingIndex;
		std::uint32_t revealageRTBindingIndex;
	};

public:
	WeightedTransparencyTechnique(Renderer* renderer);

	void AddTransparentPipeline(std::uint32_t pipelineIndex)
	{
		m_transparencyPass->AddPipeline(pipelineIndex);
	}

	void AddTransparentModelBundle(std::uint32_t bundleIndex)
	{
		m_transparencyPass->AddModelBundle(bundleIndex);
	}

	void RemoveTransparentModelBundle(std::uint32_t bundleIndex) noexcept
	{
		m_transparencyPass->RemoveModelBundle(bundleIndex);
	}
	void RemoveTransparentPipeline(std::uint32_t pipelineIndex) noexcept
	{
		m_transparencyPass->RemovePipeline(pipelineIndex);
	}

	void SetFixedDescriptors();

	void SetTransparencyPass(std::shared_ptr<ExternalRenderPass> transparencyPass) noexcept;

	void SetupTransparencyPass(std::uint32_t depthTextureIndex);

	void SetupTransparencyGraphicsPipelineSignatures(
		GraphicsPipelineManager& graphicsPipelineManager, ExternalFormat depthFormat
	);
	void SetupTransparencyCompositePipelineSignature(GraphicsPipelineManager& graphicsPipelineManager);

	void SetBuffers(ExternalResourceFactory* resourceFactory);

	void ResizeRenderTargets(std::uint32_t width, std::uint32_t height);

	void SetupCompositePassPipeline(
		std::shared_ptr<ExternalRenderPass> postProcessingPass,
		const GraphicsPipelineManager& graphicsPipelineManager
	);

	[[nodiscard]]
	static constexpr size_t GetExtraRequiredPassCount() noexcept { return 1u; }

private:
	[[nodiscard]]
	static ExternalGraphicsPipeline GetCompositePassPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;

	void BindRenderTargetTextures();

private:
	std::shared_ptr<ExternalTexture>    m_accumulationRenderTarget;
	std::shared_ptr<ExternalTexture>    m_revealageRenderTarget;
	std::shared_ptr<ExternalBuffer>     m_renderTargetBindingDataExtBuffer;
	std::shared_ptr<ExternalRenderPass> m_transparencyPass;
	std::shared_ptr<ExternalRenderPass> m_postProcessingPass;
	std::uint32_t                       m_accumulationTextureIndex;
	std::uint32_t                       m_revealageTextureIndex;
	RenderTargetBindingData             m_bindingData;
	std::uint32_t                       m_accumulationBarrierIndex;
	std::uint32_t                       m_revealageBarrierIndex;

	static constexpr std::uint32_t s_rtBindingDataBufferIndex = 0u;

	static ShaderName s_compositePassShaderName;

public:
	WeightedTransparencyTechnique(const WeightedTransparencyTechnique&) = delete;
	WeightedTransparencyTechnique& operator=(const WeightedTransparencyTechnique&) = delete;

	WeightedTransparencyTechnique(WeightedTransparencyTechnique&& other) noexcept
		: GraphicsTechniqueExtensionBase{ std::move(other) },
		m_accumulationRenderTarget{ std::move(other.m_accumulationRenderTarget) },
		m_revealageRenderTarget{ std::move(other.m_revealageRenderTarget) },
		m_renderTargetBindingDataExtBuffer{ std::move(other.m_renderTargetBindingDataExtBuffer) },
		m_transparencyPass{ std::move(other.m_transparencyPass) },
		m_postProcessingPass{ std::move(other.m_postProcessingPass) },
		m_accumulationTextureIndex{ other.m_accumulationTextureIndex },
		m_revealageTextureIndex{ other.m_revealageTextureIndex },
		m_bindingData{ other.m_bindingData },
		m_accumulationBarrierIndex{ other.m_accumulationBarrierIndex },
		m_revealageBarrierIndex{ other.m_revealageBarrierIndex }
	{}
	WeightedTransparencyTechnique& operator=(WeightedTransparencyTechnique&& other) noexcept
	{
		GraphicsTechniqueExtensionBase::operator=(std::move(other));
		m_accumulationRenderTarget         = std::move(other.m_accumulationRenderTarget);
		m_revealageRenderTarget            = std::move(other.m_revealageRenderTarget);
		m_renderTargetBindingDataExtBuffer = std::move(other.m_renderTargetBindingDataExtBuffer);
		m_transparencyPass                 = std::move(other.m_transparencyPass);
		m_postProcessingPass               = std::move(other.m_postProcessingPass);
		m_accumulationTextureIndex         = other.m_accumulationTextureIndex;
		m_revealageTextureIndex            = other.m_revealageTextureIndex;
		m_bindingData                      = other.m_bindingData;
		m_accumulationBarrierIndex         = other.m_accumulationBarrierIndex;
		m_revealageBarrierIndex            = other.m_revealageBarrierIndex;

		return *this;
	}
};
#endif
