#include <BlinnPhongLightTechnique.hpp>
#include <ExternalBindingIndices.hpp>

ShaderName BlinnPhongLightTechnique::s_lightSrcShaderName = L"NoLightShader";
ShaderName BlinnPhongLightTechnique::s_lightDstShaderName = L"BlinnPhongShader";

BlinnPhongLightTechnique::BlinnPhongLightTechnique(std::uint32_t frameCount)
	: GraphicsTechniqueExtensionBase{}, m_lightCountExtBuffer{}, m_lightInfoExtBuffer{}, m_lights{},
	m_lightStatus{}, m_lightInfoInstanceSize{ 0u }, m_frameCount{ frameCount }
{
	constexpr size_t bufferBindingCount = 2u;

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
}

void BlinnPhongLightTechnique::AddLight(std::shared_ptr<LightSource> lightSource)
{
	const size_t lightIndex = m_lights.Add(
		LightInfo
		{
			.source     = std::move(lightSource),
			.properties = BlinnPhongLightProperties{ .lightColour = DirectX::XMFLOAT3{ 1.f, 1.f, 1.f } }
		}, s_extraAllocationCount
	);

	if (lightIndex < std::size(m_lightStatus))
		m_lightStatus.resize(std::size(m_lights.Get()), true);

	m_lightStatus[lightIndex] = true;

	const NewBufferInfo_t newBufferSize = GetNewBufferSize(
		*m_lightInfoExtBuffer, sizeof(LightData), std::size(m_lights.Get()), m_frameCount,
		s_extraAllocationCount
	);

	if (newBufferSize)
	{
		const NewBufferInfo& newBufferInfo = newBufferSize.value();

		m_lightInfoInstanceSize = newBufferInfo.instanceSize;

		m_lightInfoExtBuffer->Create(newBufferInfo.bufferSize);
	}
}

void BlinnPhongLightTechnique::SetProperties(
	size_t lightIndex, const BlinnPhongLightProperties& properties
) noexcept {
	m_lights[lightIndex].properties = properties;
}

void BlinnPhongLightTechnique::SetLightColour(
	size_t lightIndex, const DirectX::XMFLOAT3& lightColour
) noexcept {
	m_lights[lightIndex].properties.lightColour = lightColour;
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

	memcpy(
		lightCountCpuStart + lightCountInstanceOffset, &activeLightCountU32, s_lightCountInstanceSize
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
	constexpr size_t externalBufferCount = 2u;

	m_externalBufferIndices.resize(externalBufferCount);

	// Light Count
	{
		const size_t bufferIndex           = s_lightCountBufferIndex;
		const size_t lightCountBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleUniform
		);

		m_lightCountExtBuffer = resourceFactory->GetExternalBufferSP(lightCountBufferIndex);

		const auto lightCountBufferIndexU32 = static_cast<std::uint32_t>(lightCountBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = lightCountBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = lightCountBufferIndexU32;

		// The Light Count bufferSize should be fixed.
		m_lightCountExtBuffer->Create(m_frameCount * s_lightCountInstanceSize);
	}

	// Light Info
	{
		const size_t bufferIndex          = s_lightInfoBufferIndex;
		const size_t lightInfoBufferIndex = resourceFactory->CreateExternalBuffer(
			ExternalBufferType::CPUVisibleSSBO
		);

		m_lightInfoExtBuffer = resourceFactory->GetExternalBufferSP(lightInfoBufferIndex);

		const auto lightInfoBufferIndexU32 = static_cast<std::uint32_t>(lightInfoBufferIndex);

		m_bufferBindingDetails[bufferIndex].descriptorInfo.externalBufferIndex = lightInfoBufferIndexU32;
		m_externalBufferIndices[bufferIndex]                                   = lightInfoBufferIndexU32;
	}
}
