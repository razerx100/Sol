#ifndef WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#define WEIGHTED_TRANSPARENCY_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalRenderPass.hpp>
#include <GraphicsPipelineManager.hpp>
#include <ModelBase.hpp>
#include <ExternalBindingIndices.hpp>

namespace Sol
{
template<class ExternalRenderPassImpl_t, class ExternalBufferImpl_t, class ExternalTextureImpl_t>
class WeightedTransparencyTechnique : public GraphicsTechniqueExtensionBase
{
	using ExternalRenderPass_t = ExternalRenderPass<ExternalRenderPassImpl_t>;
	using ExternalBuffer_t     = ExternalBuffer<ExternalBufferImpl_t>;
	using ExternalTexture_t    = ExternalTexture<ExternalTextureImpl_t>;

public:
	struct RenderTargetBindingData
	{
		std::uint32_t accumulationRTBindingIndex;
		std::uint32_t revealageRTBindingIndex;
	};

public:
	WeightedTransparencyTechnique()
		: GraphicsTechniqueExtensionBase{}, m_accumulationRenderTarget{},
	m_revealageRenderTarget{}, m_renderTargetBindingDataExtBuffer{}, m_transparencyPass{},
	m_postProcessingPass{}, m_transparencyPassIndex{ 0u }, m_accumulationTextureIndex{ 0u },
	m_revealageTextureIndex{ 0u },
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

	void AddTransparentPipeline(std::uint32_t pipelineIndex)
	{
		m_transparencyPass.AddPipeline(pipelineIndex);
	}

	template<class Renderer_t>
	void AddTransparentModelBundle(std::uint32_t bundleIndex, Renderer_t& renderer) const
	{
		renderer.AddLocalPipelinesInExternalRenderPass(
			bundleIndex, m_transparencyPassIndex
		);
	}

	void RemoveTransparentModelBundle(std::uint32_t bundleIndex) noexcept
	{
		m_transparencyPass.RemoveModelBundle(bundleIndex);
	}
	void RemoveTransparentPipeline(std::uint32_t pipelineIndex) noexcept
	{
		m_transparencyPass.RemovePipeline(pipelineIndex);
	}

	template<class Renderer_t>
	void SetFixedDescriptors(Renderer_t& renderer)
	{
		UpdateCPUBufferDescriptor(
			s_rtBindingDataBufferIndex, m_renderTargetBindingDataExtBuffer.BufferSize(),
			renderer
		);
	}

	void SetTransparencyPass(
		std::shared_ptr<ExternalRenderPassImpl_t> transparencyPass,
		std::uint32_t transparencyPassIndex
	) noexcept {
		m_transparencyPass.SetRenderPassImpl(std::move(transparencyPass));
		m_transparencyPassIndex = transparencyPassIndex;
	}

	template<class ResourceFactory_t>
	void SetupTransparencyPass(
		std::uint32_t depthTextureIndex, ResourceFactory_t& resourceFactory
	) {
		const std::uint32_t accumTargetIndex = m_transparencyPass.AddRenderTarget(
			m_accumulationTextureIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store, resourceFactory
		);

		m_transparencyPass.SetRenderTargetClearColour(
			accumTargetIndex, DirectX::XMFLOAT4{ 0.f, 0.f, 0.f, 0.f },
			resourceFactory
		);

		const std::uint32_t revealTargetIndex = m_transparencyPass.AddRenderTarget(
			m_revealageTextureIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store, resourceFactory
		);

		m_transparencyPass.SetRenderTargetClearColour(
			revealTargetIndex, DirectX::XMFLOAT4{ 1.f, 0.f, 0.f, 0.f },
			resourceFactory
		);

		m_transparencyPass.SetDepthTesting(
			depthTextureIndex, ExternalAttachmentLoadOp::Load,
			ExternalAttachmentStoreOp::DontCare, resourceFactory
		);
	}

	void SetupTransparencyGraphicsPipelineSignatures(
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

			m_renderTargetBindingDataExtBuffer.SetBufferImpl(
				resourceFactory.GetExternalBufferSP(extBufferIndex)
			);

			const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

			m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex
				= extBufferIndexU32;
			m_externalBufferIndices[bufferIndex] = extBufferIndexU32;

			m_renderTargetBindingDataExtBuffer.Create(sizeof(RenderTargetBindingData));
		}

		// Accumulation Render Target
		{
			const size_t accumulationTextureIndex = resourceFactory.CreateExternalTexture();

			m_accumulationTextureIndex = static_cast<std::uint32_t>(accumulationTextureIndex);

			m_accumulationRenderTarget.SetTextureImpl(
				resourceFactory.GetExternalTextureSP(accumulationTextureIndex)
			);
		}

		// Revealage Render Target
		{
			const size_t revealageTextureIndex = resourceFactory.CreateExternalTexture();

			m_revealageTextureIndex = static_cast<std::uint32_t>(revealageTextureIndex);

			m_revealageRenderTarget.SetTextureImpl(
				resourceFactory.GetExternalTextureSP(revealageTextureIndex)
			);
		}
	}

	template<class Renderer_t>
	void ResizeRenderTargets(std::uint32_t width, std::uint32_t height, Renderer_t& renderer)
	{
		m_accumulationRenderTarget.Create(
			width, height, ExternalFormat::R16G16B16A16_FLOAT,
			ExternalTexture2DType::RenderTarget,
			ExternalTextureCreationFlags{ .sampleTexture = true }
		);

		m_revealageRenderTarget.Create(
			width, height, ExternalFormat::R16_FLOAT, ExternalTexture2DType::RenderTarget,
			ExternalTextureCreationFlags{ .sampleTexture = true }
		);

		auto& resourceFactory = renderer.GetExternalResourceManager().GetResourceFactory();

		m_transparencyPass.ResetAttachmentReferences(resourceFactory);

		BindRenderTargetTextures(renderer);

		m_postProcessingPass.UpdateStartBarrierResource(
			m_accumulationBarrierIndex, m_accumulationTextureIndex, resourceFactory
		);
		m_postProcessingPass.UpdateStartBarrierResource(
			m_revealageBarrierIndex, m_revealageTextureIndex, resourceFactory
		);
	}

	template<class Renderer_t>
	void SetupCompositePassPipeline(
		ExternalRenderPass_t& postProcessingPass,
		const GraphicsPipelineManager& graphicsPipelineManager,
		ModelBundleBase& renderTargetQuadBundle, std::uint32_t renderTargetQuadMeshIndex,
		Renderer_t& renderer
	) {
		const std::uint32_t compositePipelineIndex = renderer.AddGraphicsPipeline(
			GetCompositePassPipeline(graphicsPipelineManager)
		);

		postProcessingPass.AddPipeline(compositePipelineIndex);

		Model quadModel{};

		// Copy the shared values if exists. As we just need a different instance with a
		// different pipeline.
		if (renderTargetQuadBundle.GetModelCount())
			quadModel.CopyCharacteristics(renderTargetQuadBundle.GetModel(0u));

		quadModel.SetMeshIndex(renderTargetQuadMeshIndex);

		renderTargetQuadBundle.AddModel(compositePipelineIndex, std::move(quadModel));
	}

	template<class ResourceFactory_t>
	void SetCompositePass(
		ExternalRenderPass_t postProcessingPass, ResourceFactory_t& resourceFactory
	) {
		m_accumulationBarrierIndex = postProcessingPass.AddStartBarrier(
			m_accumulationTextureIndex, ExternalTextureTransition::FragmentShaderReadOnly,
			resourceFactory
		);

		m_revealageBarrierIndex = postProcessingPass.AddStartBarrier(
			m_revealageTextureIndex, ExternalTextureTransition::FragmentShaderReadOnly,
			resourceFactory
		);

		m_postProcessingPass = std::move(postProcessingPass);
	}

	[[nodiscard]]
	static constexpr size_t GetExtraRequiredPassCount() noexcept { return 1u; }

private:
	[[nodiscard]]
	static ExternalGraphicsPipeline GetCompositePassPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept {
		ExternalGraphicsPipeline compositePassPipeline
			= graphicsPipelineManager.GetTransparencyCombinePassSignature();

		compositePassPipeline.SetVertexShader(
			graphicsPipelineManager.GetNoTransformVertexShader()
		);
		compositePassPipeline.SetFragmentShader(s_compositePassShaderName);

		return compositePassPipeline;
	}

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

			std::uint8_t* bindingDataCpuStart = m_renderTargetBindingDataExtBuffer.CPUHandle();

			memcpy(bindingDataCpuStart, &m_bindingData, sizeof(RenderTargetBindingData));
		}
	}

private:
	ExternalTexture_t       m_accumulationRenderTarget;
	ExternalTexture_t       m_revealageRenderTarget;
	ExternalBuffer_t        m_renderTargetBindingDataExtBuffer;
	ExternalRenderPass_t    m_transparencyPass;
	ExternalRenderPass_t    m_postProcessingPass;
	std::uint32_t           m_transparencyPassIndex;
	std::uint32_t           m_accumulationTextureIndex;
	std::uint32_t           m_revealageTextureIndex;
	RenderTargetBindingData m_bindingData;
	std::uint32_t           m_accumulationBarrierIndex;
	std::uint32_t           m_revealageBarrierIndex;

	static constexpr std::uint32_t s_rtBindingDataBufferIndex = 0u;

	inline static ShaderName s_compositePassShaderName = L"WeightedTransparencyCompositeShader";

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
		m_transparencyPassIndex{ other.m_transparencyPassIndex },
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
		m_transparencyPassIndex            = other.m_transparencyPassIndex;
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
