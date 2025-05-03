#include <BlinnPhongLightTechnique.hpp>
#include <ExternalBindingIndices.hpp>
#include <ConversionUtilities.hpp>

namespace Sol
{
ShaderName BlinnPhongLightTechnique::s_opaqueLightSrcShaderName      = L"NoLightOpaqueShader";
ShaderName BlinnPhongLightTechnique::s_opaqueLightDstShaderName      = L"BlinnPhongOpaqueShader";

ShaderName BlinnPhongLightTechnique::s_transparentLightSrcShaderName = L"NoLightTransparentShader";
ShaderName BlinnPhongLightTechnique::s_transparentLightDstShaderName = L"BlinnPhongTransparentShader";

BlinnPhongLightTechnique::BlinnPhongLightTechnique(std::uint32_t frameCount)
	: GraphicsTechniqueExtensionBase{}, m_lightCountExtBuffer{}, m_lightInfoExtBuffer{},
	m_materials{}, m_lights{}, m_lightStatus{}, m_lightInfoInstanceSize{ 0u },
	m_frameCount{ frameCount }
{
	constexpr size_t bufferBindingCount = 3u;

	m_bufferBindingDetails.resize(bufferBindingCount);

	m_bufferBindingDetails[s_lightCountBufferIndex] = ExternalBufferBindingDetails
	{
		.layoutInfo =
		{
			.bindingIndex = ExternalBinding::s_lightCount,
			.type         = ExternalBufferType::CPUVisibleUniform
		}
	};

	m_bufferBindingDetails[s_lightInfoBufferIndex] = ExternalBufferBindingDetails
	{
		.layoutInfo =
		{
			.bindingIndex = ExternalBinding::s_lightInfo,
			.type         = ExternalBufferType::CPUVisibleSSBO
		}
	};

	m_bufferBindingDetails[s_materialBufferIndex] = ExternalBufferBindingDetails
	{
		.layoutInfo =
		{
			.bindingIndex = ExternalBinding::s_material,
			.type         = ExternalBufferType::CPUVisibleSSBO
		}
	};
}

void BlinnPhongLightTechnique::RemoveMaterial(size_t index)
{
	m_materials.Remove(index);
}

void BlinnPhongLightTechnique::SetProperties(
	size_t lightIndex, const BlinnPhongLightProperties& properties
) noexcept {
	m_lights[lightIndex].properties           = properties;
	m_lights[lightIndex].properties.direction = NormaliseFloat3(properties.direction);
}

void BlinnPhongLightTechnique::SetLightColour(
	size_t lightIndex, const DirectX::XMFLOAT3& ambient, const DirectX::XMFLOAT3& diffuse,
	const DirectX::XMFLOAT3& specular
) noexcept {
	m_lights[lightIndex].properties.ambient  = ambient;
	m_lights[lightIndex].properties.diffuse  = diffuse;
	m_lights[lightIndex].properties.specular = specular;
}

void BlinnPhongLightTechnique::SetDirection(
	size_t lightIndex, const DirectX::XMFLOAT3& direction
) noexcept {
	m_lights[lightIndex].properties.direction = NormaliseFloat3(direction);
}

void BlinnPhongLightTechnique::SetAttenuationCoefficients(
	size_t lightIndex, float constant, float linear, float quadratic
) noexcept {
	m_lights[lightIndex].properties.constant  = constant;
	m_lights[lightIndex].properties.linear    = linear;
	m_lights[lightIndex].properties.quadratic = quadratic;
}

void BlinnPhongLightTechnique::SetCutoffs(
	size_t lightIndex, float innerCutoff, float outerCutoff
) noexcept {
	m_lights[lightIndex].properties.innerCutoff = innerCutoff;
	m_lights[lightIndex].properties.outerCutoff = outerCutoff;
}

void BlinnPhongLightTechnique::SetType(size_t lightIndex, BlinnPhongLightType type) noexcept
{
	m_lights[lightIndex].properties.lightType = static_cast<std::uint32_t>(type);
}

void BlinnPhongLightTechnique::UpdateCPUData(size_t frameIndex) noexcept
{
	const std::vector<LightInfo>& lights = m_lights.Get();

	// We care about the light indices on the CPU but don't care about it on the GPU.
	// So, to avoid unnecessary branches in the Fragment shader, we are going to rewrite the buffer and the
	// element count every frame with only the active lights.
	std::uint8_t* lightInfoCpuStart      = m_lightInfoExtBuffer->CPUHandle();
	const size_t lightInfoInstanceOffset = m_lightInfoInstanceSize * frameIndex;
	size_t activeLightIndex              = 0u;

	for (size_t index = 0u; index < std::size(lights); ++index)
	{
		if (m_lightStatus[index])
		{
			const LightInfo& lightInfo = lights[index];
			const LightSource& light   = lightInfo.source;
			const size_t lightOffset   = sizeof(LightData) * activeLightIndex;

			const LightData lightData
			{
				.lightPosition = light.GetPosition(),
				.properties    = lightInfo.properties
			};

			memcpy(
				lightInfoCpuStart + lightInfoInstanceOffset + lightOffset,
				&lightData, sizeof(LightData)
			);

			++activeLightIndex;
		}
	}

	// Copy the new light count.
	std::uint8_t* lightCountCpuStart      = m_lightCountExtBuffer->CPUHandle();
	const size_t lightCountInstanceOffset = s_lightCountInstanceSize * frameIndex;
	const auto activeLightCountU32        = static_cast<std::uint32_t>(activeLightIndex);

	// Can't use the lightCount instance size here as it is bigger than the data size.
	memcpy(
		lightCountCpuStart + lightCountInstanceOffset, &activeLightCountU32, sizeof(std::uint32_t)
	);
}

void BlinnPhongLightTechnique::RemoveLight(size_t index) noexcept
{
	m_lights.RemoveElement(index);

	m_lightStatus[index] = false;
}

void BlinnPhongLightTechnique::ToggleLight(size_t index, bool value) noexcept
{
	m_lightStatus[index] = value;
}

ExternalGraphicsPipeline BlinnPhongLightTechnique::GetOpaqueLightSrcPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline opaqueNoLightPipeline
		= graphicsPipelineManager.GetMainPassOpaqueSignature();

	opaqueNoLightPipeline.SetVertexShader(graphicsPipelineManager.GetDefaultVertexShader());
	opaqueNoLightPipeline.SetFragmentShader(s_opaqueLightSrcShaderName);

	return opaqueNoLightPipeline;
}

ExternalGraphicsPipeline BlinnPhongLightTechnique::GetOpaqueLightDstPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline opaqueLightPipeline = graphicsPipelineManager.GetMainPassOpaqueSignature();

	opaqueLightPipeline.SetVertexShader(graphicsPipelineManager.GetDefaultVertexShader());
	opaqueLightPipeline.SetFragmentShader(s_opaqueLightDstShaderName);

	return opaqueLightPipeline;
}

ExternalGraphicsPipeline BlinnPhongLightTechnique::GetTransparentLightSrcPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline transparentNoLightPipeline
		= graphicsPipelineManager.GetTransparencyPassSignature();

	transparentNoLightPipeline.SetVertexShader(graphicsPipelineManager.GetDefaultVertexShader());
	transparentNoLightPipeline.SetFragmentShader(s_transparentLightSrcShaderName);

	return transparentNoLightPipeline;
}

ExternalGraphicsPipeline BlinnPhongLightTechnique::GetTransparentLightDstPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline transparentLightPipeline
		= graphicsPipelineManager.GetTransparencyPassSignature();

	transparentLightPipeline.SetVertexShader(graphicsPipelineManager.GetDefaultVertexShader());
	transparentLightPipeline.SetFragmentShader(s_transparentLightDstShaderName);

	return transparentLightPipeline;
}
}
