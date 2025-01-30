#ifndef BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#define BLINN_PHONG_LIGHT_TECHNIQUE_HPP_
#include <GraphicsTechniqueExtensionBase.hpp>
#include <ExternalResourceFactory.hpp>
#include <LightSource.hpp>
#include <ReusableVector.hpp>

struct BlinnPhongLightProperties
{
	DirectX::XMFLOAT3 lightColour{ 1.f, 1.f, 1.f };
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

	void AddLight(std::shared_ptr<LightSource> lightSource);

	void RemoveLight(size_t index) noexcept;
	void ToggleLight(size_t index, bool value) noexcept;

	void SetProperties(size_t lightIndex, const BlinnPhongLightProperties& properties) noexcept;
	void SetLightColour(size_t lightIndex, const DirectX::XMFLOAT3& lightColour) noexcept;

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
		BlinnPhongLightProperties properties;
	};

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

	static constexpr size_t s_lightCountInstanceSize = sizeof(std::uint32_t);
	static constexpr size_t s_extraAllocationCount = 4u;

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
