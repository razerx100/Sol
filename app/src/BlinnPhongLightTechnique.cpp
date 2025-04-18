#include <BlinnPhongLightTechnique.hpp>
#include <ExternalBindingIndices.hpp>
#include <ConversionUtilities.hpp>

ShaderName BlinnPhongLightTechnique::s_opaqueLightSrcShaderName      = L"NoLightOpaqueShader";
ShaderName BlinnPhongLightTechnique::s_opaqueLightDstShaderName      = L"BlinnPhongOpaqueShader";

ShaderName BlinnPhongLightTechnique::s_transparentLightSrcShaderName = L"NoLightTransparentShader";
ShaderName BlinnPhongLightTechnique::s_transparentLightDstShaderName = L"BlinnPhongTransparentShader";

BlinnPhongLightTechnique::BlinnPhongLightTechnique(Renderer* renderer, std::uint32_t frameCount)
	: GraphicsTechniqueExtensionBase{ renderer }, m_lightCountExtBuffer{}, m_lightInfoExtBuffer{},
	m_materials{}, m_lights{}, m_lightStatus{}, m_lightInfoInstanceSize{ 0u }, m_frameCount{ frameCount }
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

std::uint32_t BlinnPhongLightTechnique::AddLight(
	std::shared_ptr<LightSource> lightSource, BlinnPhongLightType type
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
		m_renderer->WaitForGPUToFinish();

		m_lightInfoExtBuffer->Create(newBufferInfo.bufferSize);

		// The new light data will be copied on the next frame. And we don't
		// need to worry about the GPU waiting for that. But we will need to
		// update the descriptor if the buffer size is increased.
		UpdateLightInfoDescriptors();
	}

	return static_cast<std::uint32_t>(lightIndex);
}

std::uint32_t BlinnPhongLightTechnique::AddMaterial(const BlinnPhongMaterial& material)
{
	m_renderer->WaitForGPUToFinish();

	const auto index = static_cast<std::uint32_t>(m_materials.Add(material));

	UpdateMaterialDescriptor();

	return index;
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
			const Light_t& light       = lightInfo.source;
			const size_t lightOffset   = sizeof(LightData) * activeLightIndex;

			const LightData lightData
			{
				.lightPosition = light->GetPosition(),
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

void BlinnPhongLightTechnique::SetBuffers(ExternalResourceFactory* resourceFactory)
{
	constexpr size_t externalBufferCount = 3u;

	m_externalBufferIndices.resize(externalBufferCount);

	// Light Count
	{
		const size_t bufferIndex    = s_lightCountBufferIndex;
		const size_t extBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleUniform
		);

		m_lightCountExtBuffer = resourceFactory->GetExternalBufferSP(extBufferIndex);

		const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = extBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = extBufferIndexU32;

		// The Light Count bufferSize should be fixed.
		m_lightCountExtBuffer->Create(m_frameCount * s_lightCountInstanceSize);
	}

	// Light Info
	{
		const size_t bufferIndex    = s_lightInfoBufferIndex;
		const size_t extBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleSSBO
		);

		m_lightInfoExtBuffer         = resourceFactory->GetExternalBufferSP(extBufferIndex);

		const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = extBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = extBufferIndexU32;
	}

	// Material
	{
		const size_t bufferIndex    = s_materialBufferIndex;
		const size_t extBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleSSBO
		);

		m_materials.SetCPUExtBuffer(
			resourceFactory->GetExternalBufferSP(extBufferIndex)
		);

		const auto extBufferIndexU32 = static_cast<std::uint32_t>(extBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = extBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = extBufferIndexU32;
	}
}

void BlinnPhongLightTechnique::UpdateLightCountDescriptors()
{
	for (size_t frameIndex = 0u; frameIndex < m_frameCount; ++frameIndex)
		UpdateCPUBufferDescriptor(s_lightCountBufferIndex, frameIndex, s_lightCountInstanceSize);
}

void BlinnPhongLightTechnique::UpdateLightInfoDescriptors()
{
	for (size_t frameIndex = 0u; frameIndex < m_frameCount; ++frameIndex)
		UpdateCPUBufferDescriptor(s_lightInfoBufferIndex, frameIndex, m_lightInfoInstanceSize);
}

void BlinnPhongLightTechnique::UpdateMaterialDescriptor()
{
	UpdateCPUBufferDescriptor(s_materialBufferIndex, m_materials.GetExtBuffer()->BufferSize());
}

void BlinnPhongLightTechnique::SetFixedDescriptors()
{
	UpdateLightCountDescriptors();
}

ExternalGraphicsPipeline BlinnPhongLightTechnique::GetOpaqueLightSrcPipeline(
	const GraphicsPipelineManager& graphicsPipelineManager
) noexcept {
	ExternalGraphicsPipeline opaqueNoLightPipeline = graphicsPipelineManager.GetMainPassOpaqueSignature();

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
