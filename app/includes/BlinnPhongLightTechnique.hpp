#ifndef BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#define BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalResourceFactory.hpp>
#include <LightSource.hpp>
#include <ReusableVector.hpp>

// The alphas aren't needed but keeping them for alignment reasons. And it would be
// easier to deal in Float3 all the time.
struct BlinnPhongLightProperties
{
	DirectX::XMFLOAT3 ambient{ 1.f, 1.f, 1.f };
	float             ambientAlpha = 1.f;
	DirectX::XMFLOAT3 diffuse{ 1.f, 1.f, 1.f };
	float             diffuseAlpha = 1.f;
	DirectX::XMFLOAT3 specular{ 1.f, 1.f, 1.f };
	float             specularAlpha = 1.f;
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
	std::uint32_t AddLight(std::shared_ptr<LightSource> lightSource);

	void RemoveLight(size_t index) noexcept;
	void ToggleLight(size_t index, bool value) noexcept;

	void SetProperties(size_t lightIndex, const BlinnPhongLightProperties& properties) noexcept;
	void SetLightColour(
		size_t lightIndex, const DirectX::XMFLOAT3& ambient, const DirectX::XMFLOAT3& diffuse,
		const DirectX::XMFLOAT3& specular
	) noexcept;

	void SetFixedDescriptors();

	void UpdateCPUData(size_t frameIndex) noexcept override;

	void SetBuffers(ExternalResourceFactory* resourceFactory);

	[[nodiscard]]
	static const ShaderName& GetLightSrcShaderName() noexcept { return s_lightSrcShaderName; }
	[[nodiscard]]
	static const ShaderName& GetLightDstShaderName() noexcept { return s_lightDstShaderName; }

	[[nodiscard]]
	LightSource& GetLightSource(size_t index) noexcept { return *m_lights[index].source; }
	[[nodiscard]]
	const LightSource& GetLightSource(size_t index) const noexcept { return *m_lights[index].source; }

private:
	struct LightData
	{
		DirectX::XMFLOAT3         lightPosition;
		float                     padding1;
		BlinnPhongLightProperties properties;
		// Vec3 in GLSL are Vec4, so need the padding.
	};

private:
	void UpdateLightCountDescriptors();
	void UpdateLightInfoDescriptors();

private:
	std::shared_ptr<ExternalBuffer> m_lightCountExtBuffer;
	std::shared_ptr<ExternalBuffer> m_lightInfoExtBuffer;
	ReusableVector<LightInfo>       m_lights;
	std::vector<bool>               m_lightStatus;
	size_t                          m_lightInfoInstanceSize;
	std::uint32_t                   m_frameCount;

	static ShaderName s_lightSrcShaderName;
	static ShaderName s_lightDstShaderName;

	static constexpr std::uint32_t s_lightCountBufferIndex = 0u;
	static constexpr std::uint32_t s_lightInfoBufferIndex  = 1u;

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
		m_lights                = std::move(other.m_lights);
		m_lightStatus           = std::move(other.m_lightStatus);
		m_lightInfoInstanceSize = other.m_lightInfoInstanceSize;
		m_frameCount            = other.m_frameCount;

		return *this;
	}
};
#endif
