#ifndef WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#define WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalRenderPass.hpp>
#include <GraphicsPipelineManager.hpp>
#include <ModelBase.hpp>

namespace Sol
{
class WeightedTransparencyTechnique : public GraphicsTechniqueExtensionBase
{
public:
	struct RenderTargetBindingData
	{
		std::uint32_t accumulationRTBindingIndex;
		std::uint32_t revealageRTBindingIndex;
	};

public:
	WeightedTransparencyTechnique();

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

	template<class Renderer_t>
	void SetFixedDescriptors(Renderer_t& renderer)
	{
		UpdateCPUBufferDescriptor(
			s_rtBindingDataBufferIndex, m_renderTargetBindingDataExtBuffer->BufferSize(),
			renderer
		);
	}

	void SetTransparencyPass(std::shared_ptr<ExternalRenderPass> transparencyPass) noexcept;

	void SetupTransparencyPass(std::uint32_t depthTextureIndex);

	void SetupTransparencyGraphicsPipelineSignatures(
		GraphicsPipelineManager& graphicsPipelineManager, ExternalFormat depthFormat
	);

	template<class Renderer_t>
	void SetupTransparencyCompositePipelineSignature(
		GraphicsPipelineManager& graphicsPipelineManager, Renderer_t& renderer
	) {
		ExternalGraphicsPipeline transparencyCombineSignature{};

		transparencyCombineSignature.AddRenderTarget(
			renderer.GetSwapchainFormat(),
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

	template<class ResourceFactory_t>
	void SetBuffers(ResourceFactory_t& resourceFactory)
	{
		constexpr size_t externalBufferCount = 1u;

		m_externalBufferIndices.resize(externalBufferCount);

		// Render Target Binding Data
		{
			const size_t bufferIndex    = s_rtBindingDataBufferIndex;
			const size_t extBufferIndex = resourceFactory.CreateExternalBuffer(
				ExternalBufferType::CPUVisibleUniform
			);

			m_renderTargetBindingDataExtBuffer = resourceFactory.GetExternalBufferSP(
				extBufferIndex
			);

			const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

			m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex
				= extBufferIndexU32;
			m_externalBufferIndices[bufferIndex] = extBufferIndexU32;

			m_renderTargetBindingDataExtBuffer->Create(sizeof(RenderTargetBindingData));
		}

		// Accumulation Render Target
		{
			const size_t accumulationTextureIndex = resourceFactory.CreateExternalTexture();

			m_accumulationTextureIndex = static_cast<std::uint32_t>(accumulationTextureIndex);

			m_accumulationRenderTarget = resourceFactory.GetExternalTextureSP(
				accumulationTextureIndex
			);
		}

		// Revealage Render Target
		{
			const size_t revealageTextureIndex = resourceFactory.CreateExternalTexture();

			m_revealageTextureIndex = static_cast<std::uint32_t>(revealageTextureIndex);

			m_revealageRenderTarget = resourceFactory.GetExternalTextureSP(revealageTextureIndex);
		}
	}

	template<class Renderer_t>
	void ResizeRenderTargets(std::uint32_t width, std::uint32_t height, Renderer_t& renderer)
	{
		m_accumulationRenderTarget->Create(
			width, height, ExternalFormat::R16G16B16A16_FLOAT,
			ExternalTexture2DType::RenderTarget,
			ExternalTextureCreationFlags{ .sampleTexture = true }
		);

		m_revealageRenderTarget->Create(
			width, height, ExternalFormat::R16_FLOAT, ExternalTexture2DType::RenderTarget,
			ExternalTextureCreationFlags{ .sampleTexture = true }
		);

		m_transparencyPass->ResetAttachmentReferences();

		BindRenderTargetTextures(renderer);

		m_postProcessingPass->UpdateStartBarrierResource(
			m_accumulationBarrierIndex, m_accumulationTextureIndex
		);
		m_postProcessingPass->UpdateStartBarrierResource(
			m_revealageBarrierIndex, m_revealageTextureIndex
		);
	}

	template<class Renderer_t>
	void SetupCompositePassPipeline(
		ExternalRenderPass* postProcessingPass,
		const GraphicsPipelineManager& graphicsPipelineManager,
		ModelBundleBase& renderTargetQuadBundle, std::uint32_t renderTargetQuadMeshIndex,
		Renderer_t& renderer
	) {
		const std::uint32_t compositePipelineIndex = renderer.AddGraphicsPipeline(
			GetCompositePassPipeline(graphicsPipelineManager)
		);

		postProcessingPass->AddPipeline(compositePipelineIndex);

		auto quadModel = std::make_shared<Model>();

		const auto& models = renderTargetQuadBundle.GetModels();

		// Copy the shared values if exists. As we just need a different instance with a different pipeline.
		if (!std::empty(models))
			quadModel->CopyCharacteristics(*models[0]);

		quadModel->SetMeshIndex(renderTargetQuadMeshIndex);

		renderTargetQuadBundle.AddModel(compositePipelineIndex, std::move(quadModel));
	}

	void SetCompositePass(std::shared_ptr<ExternalRenderPass> postProcessingPass);

	[[nodiscard]]
	static constexpr size_t GetExtraRequiredPassCount() noexcept { return 1u; }

private:
	[[nodiscard]]
	static ExternalGraphicsPipeline GetCompositePassPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;

	template<class Renderer_t>
	void BindRenderTargetTextures(Renderer_t& renderer)
	{
		constexpr auto uint32Max    = std::numeric_limits<std::uint32_t>::max();

		const bool areTexturesBound = m_bindingData.accumulationRTBindingIndex != uint32Max
			&& m_bindingData.revealageRTBindingIndex != uint32Max;

		if (areTexturesBound)
		{
			renderer.RebindExternalTexture(
				m_accumulationTextureIndex, m_bindingData.accumulationRTBindingIndex
			);
			renderer.RebindExternalTexture(
				m_revealageTextureIndex, m_bindingData.revealageRTBindingIndex
			);
		}
		else
		{
			m_bindingData.accumulationRTBindingIndex = renderer.BindExternalTexture(
				m_accumulationTextureIndex
			);
			m_bindingData.revealageRTBindingIndex
				= renderer.BindExternalTexture(m_revealageTextureIndex);

			std::uint8_t* bindingDataCpuStart = m_renderTargetBindingDataExtBuffer->CPUHandle();

			memcpy(bindingDataCpuStart, &m_bindingData, sizeof(RenderTargetBindingData));
		}
	}

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
}
#endif
