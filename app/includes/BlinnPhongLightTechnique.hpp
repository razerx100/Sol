#ifndef BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#define BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <GraphicsPipelineManager.hpp>
#include <LightSource.hpp>
#include <ReusableVector.hpp>
#include <ReusableExtBuffer.hpp>

namespace Sol
{
// The alphas aren't needed but keeping them for alignment reasons. And it would be
// easier to deal in Float3 all the time.
struct BlinnPhongLightProperties
{
	DirectX::XMFLOAT3 direction{ 0.f, 0.f, 0.f };
	float             padding = 0.f;
	DirectX::XMFLOAT3 ambient{ 1.f, 1.f, 1.f };
	float             innerCutoff = 1.f;
	DirectX::XMFLOAT3 diffuse{ 1.f, 1.f, 1.f };
	float             diffuseAlpha = 1.f;
	DirectX::XMFLOAT3 specular{ 1.f, 1.f, 1.f };
	float             outerCutoff = 1.f;
	// Attenuation co-efficients
	float             constant    = 1.f;
	float             linear      = 0.f;
	float             quadratic   = 0.f;
	std::uint32_t     lightType   = 0u; // 0 for Directional, 1 for Point and 2 for Spotlight
};

enum class BlinnPhongLightType
{
	Directional,
	Point,
	Spotlight
};

struct BlinnPhongMaterial
{
	DirectX::XMFLOAT4 ambient{};
	DirectX::XMFLOAT4 diffuse{};
	DirectX::XMFLOAT4 specular{};
	float             shininess = 1.f;
	float             padding[3] = {};
};

class BlinnPhongLightTechnique : public GraphicsTechniqueExtensionBase
{
	using Light_t = std::shared_ptr<LightSource>;

	struct LightInfo
	{
		Light_t                   source;
		BlinnPhongLightProperties properties;
	};

public:
	BlinnPhongLightTechnique(std::uint32_t frameCount);

	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t AddLight(
		Renderer_t& renderer, std::shared_ptr<LightSource> lightSource, BlinnPhongLightType type
	) {
		const size_t lightIndex = m_lights.Add(
			LightInfo
			{
				.source     = std::move(lightSource),
				.properties = BlinnPhongLightProperties
				{
					.direction   = DirectX::XMFLOAT3{ 0.f, 0.f, 0.f },
					.ambient     = DirectX::XMFLOAT3{ 1.f, 1.f, 1.f },
					.innerCutoff = 1.f,
					.diffuse     = DirectX::XMFLOAT3{ 1.f, 1.f, 1.f },
					.specular    = DirectX::XMFLOAT3{ 1.f, 1.f, 1.f },
					.outerCutoff = 1.f,
					.constant    = 1.f,
					.linear      = 0.f,
					.quadratic   = 0.f,
					.lightType   = static_cast<std::uint32_t>(type)
				}
			}, s_extraAllocationCount
		);

		if (lightIndex >= std::size(m_lightStatus))
			m_lightStatus.resize(std::size(m_lights), false);

		m_lightStatus[lightIndex] = true;

		const NewBufferInfo_t newBufferSize = GetNewBufferSize(
			*m_lightInfoExtBuffer, sizeof(LightData), std::size(m_lights), m_frameCount,
			s_extraAllocationCount
		);

		if (newBufferSize)
		{
			const NewBufferInfo& newBufferInfo = newBufferSize.value();

			m_lightInfoInstanceSize = newBufferInfo.instanceSize;

			// Should wait for the gpu to finish before recreating the buffer.
			renderer.WaitForGPUToFinish();

			m_lightInfoExtBuffer->Create(newBufferInfo.bufferSize);

			// The new light data will be copied on the next frame. And we don't
			// need to worry about the GPU waiting for that. But we will need to
			// update the descriptor if the buffer size is increased.
			UpdateLightInfoDescriptors(renderer);
		}

		return static_cast<std::uint32_t>(lightIndex);
	}

	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t AddMaterial(Renderer_t& renderer, const BlinnPhongMaterial& material)
	{
		renderer.WaitForGPUToFinish();

		const auto index = static_cast<std::uint32_t>(m_materials.Add(material));

		UpdateMaterialDescriptor(renderer);

		return index;
	}

	void RemoveLight(size_t index) noexcept;
	void ToggleLight(size_t index, bool value) noexcept;

	void SetProperties(size_t lightIndex, const BlinnPhongLightProperties& properties) noexcept;

	void SetLightColour(
		size_t lightIndex, const DirectX::XMFLOAT3& ambient, const DirectX::XMFLOAT3& diffuse,
		const DirectX::XMFLOAT3& specular
	) noexcept;

	// Works for Directional and Spotlight.
	void SetDirection(size_t lightIndex, const DirectX::XMFLOAT3& direction) noexcept;
	// Works for Point Light and Spotlight.
	void SetAttenuationCoefficients(
		size_t lightIndex, float constant, float linear, float quadratic
	) noexcept;
	// Works only for SpotLight
	void SetCutoffs(size_t lightIndex, float innerCutoff, float outerCutoff) noexcept;
	void SetType(size_t lightIndex, BlinnPhongLightType type) noexcept;

	void RemoveMaterial(size_t index);

	template<class Renderer_t>
	void SetFixedDescriptors(Renderer_t& renderer)
	{
		UpdateLightCountDescriptors(renderer);
	}

	void UpdateCPUData(size_t frameIndex) noexcept override;

	template<class ResourceFactory_t>
	void SetBuffers(ResourceFactory_t& resourceFactory)
	{
		constexpr size_t externalBufferCount = 3u;

		m_externalBufferIndices.resize(externalBufferCount);

		// Light Count
		{
			const size_t bufferIndex    = s_lightCountBufferIndex;
			const size_t extBufferIndex = resourceFactory.CreateExternalBuffer(
				ExternalBufferType::CPUVisibleUniform
			);

			m_lightCountExtBuffer = resourceFactory.GetExternalBufferSP(extBufferIndex);

			const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

			m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex
				= extBufferIndexU32;
			m_externalBufferIndices[bufferIndex] = extBufferIndexU32;

			// The Light Count bufferSize should be fixed.
			m_lightCountExtBuffer->Create(m_frameCount * s_lightCountInstanceSize);
		}

		// Light Info
		{
			const size_t bufferIndex    = s_lightInfoBufferIndex;
			const size_t extBufferIndex = resourceFactory.CreateExternalBuffer(
				ExternalBufferType::CPUVisibleSSBO
			);

			m_lightInfoExtBuffer = resourceFactory.GetExternalBufferSP(extBufferIndex);

			const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

			m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex
				= extBufferIndexU32;
			m_externalBufferIndices[bufferIndex] = extBufferIndexU32;
		}

		// Material
		{
			const size_t bufferIndex    = s_materialBufferIndex;
			const size_t extBufferIndex = resourceFactory.CreateExternalBuffer(
				ExternalBufferType::CPUVisibleSSBO
			);

			m_materials.SetCPUExtBuffer(
				resourceFactory.GetExternalBufferSP(extBufferIndex)
			);

			const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

			m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex
				= extBufferIndexU32;
			m_externalBufferIndices[bufferIndex] = extBufferIndexU32;
		}
	}

	// For generic opaque light objects without light shading. With the main pass' signature.
	[[nodiscard]]
	static ExternalGraphicsPipeline GetOpaqueLightSrcPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;
	// For generic opaque light objects with light shading. With the main pass' signature.
	[[nodiscard]]
	static ExternalGraphicsPipeline GetOpaqueLightDstPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;

	// For transparent light objects without light shading. Requires the transparency extension.
	[[nodiscard]]
	static ExternalGraphicsPipeline GetTransparentLightSrcPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;
	// For transparent light objects wit light shading. Requires the transparency extension.
	[[nodiscard]]
	static ExternalGraphicsPipeline GetTransparentLightDstPipeline(
		const GraphicsPipelineManager& graphicsPipelineManager
	) noexcept;

	[[nodiscard]]
	auto&& GetLightSource(this auto&& self, size_t index) noexcept
	{
		return std::forward_like<decltype(self)>(*self.m_lights[index].source);
	}

private:
	struct LightData
	{
		DirectX::XMFLOAT3         lightPosition;
		float                     padding;
		BlinnPhongLightProperties properties;
		// Vec3 in GLSL are Vec4, so need the padding.
	};

private:
	template<class Renderer_t>
	void UpdateLightCountDescriptors(Renderer_t& renderer)
	{
		for (size_t frameIndex = 0u; frameIndex < m_frameCount; ++frameIndex)
			UpdateCPUBufferDescriptor(
				s_lightCountBufferIndex, frameIndex, s_lightCountInstanceSize, renderer
			);
	}

	template<class Renderer_t>
	void UpdateLightInfoDescriptors(Renderer_t& renderer)
	{
		for (size_t frameIndex = 0u; frameIndex < m_frameCount; ++frameIndex)
			UpdateCPUBufferDescriptor(
				s_lightInfoBufferIndex, frameIndex, m_lightInfoInstanceSize, renderer
			);
	}

	template<class Renderer_t>
	void UpdateMaterialDescriptor(Renderer_t& renderer)
	{
		UpdateCPUBufferDescriptor(
			s_materialBufferIndex, m_materials.GetExtBuffer()->BufferSize(), renderer
		);
	}

private:
	std::shared_ptr<ExternalBuffer>          m_lightCountExtBuffer;
	std::shared_ptr<ExternalBuffer>          m_lightInfoExtBuffer;
	ReusableCPUExtBuffer<BlinnPhongMaterial> m_materials;
	Callisto::ReusableVector<LightInfo>      m_lights;
	std::vector<bool>                        m_lightStatus;
	size_t                                   m_lightInfoInstanceSize;
	std::uint32_t                            m_frameCount;

	static ShaderName s_opaqueLightSrcShaderName;
	static ShaderName s_opaqueLightDstShaderName;
	static ShaderName s_transparentLightSrcShaderName;
	static ShaderName s_transparentLightDstShaderName;

	static constexpr std::uint32_t s_lightCountBufferIndex = 0u;
	static constexpr std::uint32_t s_lightInfoBufferIndex  = 1u;
	static constexpr std::uint32_t s_materialBufferIndex   = 2u;

	// Since the light count is set as a Uniform buffer, it must be 256 bytes aligned.
	static constexpr size_t s_lightCountInstanceSize = 256u;
	static constexpr size_t s_extraAllocationCount   = 4u;

public:
	BlinnPhongLightTechnique(const BlinnPhongLightTechnique&) = delete;
	BlinnPhongLightTechnique& operator=(const BlinnPhongLightTechnique&) = delete;

	BlinnPhongLightTechnique(BlinnPhongLightTechnique&& other) noexcept
		: GraphicsTechniqueExtensionBase{ std::move(other) },
		m_lightCountExtBuffer{ std::move(other.m_lightCountExtBuffer) },
		m_lightInfoExtBuffer{ std::move(other.m_lightInfoExtBuffer) },
		m_materials{ std::move(other.m_materials) },
		m_lights{ std::move(other.m_lights) },
		m_lightStatus{ std::move(other.m_lightStatus) },
		m_lightInfoInstanceSize{ other.m_lightInfoInstanceSize },
		m_frameCount{ other.m_frameCount }
	{}
	BlinnPhongLightTechnique& operator=(BlinnPhongLightTechnique&& other) noexcept
	{
		GraphicsTechniqueExtensionBase::operator=(std::move(other));
		m_lightCountExtBuffer   = std::move(other.m_lightCountExtBuffer);
		m_lightInfoExtBuffer    = std::move(other.m_lightInfoExtBuffer);
		m_materials             = std::move(other.m_materials);
		m_lights                = std::move(other.m_lights);
		m_lightStatus           = std::move(other.m_lightStatus);
		m_lightInfoInstanceSize = other.m_lightInfoInstanceSize;
		m_frameCount            = other.m_frameCount;

		return *this;
	}
};
}
#endif
