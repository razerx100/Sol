#ifndef BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#define BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalResourceFactory.hpp>
#include <GraphicsPipelineManager.hpp>
#include <LightSource.hpp>
#include <ReusableVector.hpp>
#include <ReusableExtBuffer.hpp>

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
	BlinnPhongLightTechnique(Renderer* renderer, std::uint32_t frameCount);

	[[nodiscard]]
	std::uint32_t AddLight(std::shared_ptr<LightSource> lightSource, BlinnPhongLightType type);
	[[nodiscard]]
	std::uint32_t AddMaterial(const BlinnPhongMaterial& material);

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

	void SetFixedDescriptors();

	void UpdateCPUData(size_t frameIndex) noexcept override;

	void SetBuffers(ExternalResourceFactory* resourceFactory);

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
	void UpdateLightCountDescriptors();
	void UpdateLightInfoDescriptors();
	void UpdateMaterialDescriptor();

private:
	std::shared_ptr<ExternalBuffer>          m_lightCountExtBuffer;
	std::shared_ptr<ExternalBuffer>          m_lightInfoExtBuffer;
	ReusableCPUExtBuffer<BlinnPhongMaterial> m_materials;
	ReusableVector<LightInfo>                m_lights;
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
#endif
