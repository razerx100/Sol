#include <GraphicsPipelineManager.hpp>

GraphicsPipelineManager::GraphicsPipelineManager(RenderEngineType engineType)
	: m_mainPassOpaqueSignature{}, m_transparencyPassSignature{}, m_transparencyCombinePassSignature{},
	m_engineType{ engineType }
{}

void GraphicsPipelineManager::SetMainPassOpaqueSignature(
	ExternalGraphicsPipeline&& pipelineSignature
) noexcept {
	m_mainPassOpaqueSignature = std::move(pipelineSignature);
}

void GraphicsPipelineManager::SetTransparencyPassSignature(
	ExternalGraphicsPipeline&& pipelineSignature
) noexcept {
	m_transparencyPassSignature = std::move(pipelineSignature);
}

void GraphicsPipelineManager::SetTransparencyCombinePassSignature(
	ExternalGraphicsPipeline&& pipelineSignature
) noexcept {
	m_transparencyCombinePassSignature = std::move(pipelineSignature);
}

ShaderName GraphicsPipelineManager::GetDefaultVertexShader() const noexcept
{
	ShaderName vertexShader{};

	if (m_engineType == RenderEngineType::MeshDraw)
		vertexShader.SetName(L"MeshShaderMSIndividual");
	else if (m_engineType == RenderEngineType::IndirectDraw)
		vertexShader.SetName(L"VertexShaderIndirect");
	else if (m_engineType == RenderEngineType::IndividualDraw)
		vertexShader.SetName(L"VertexShaderIndividual");

	return vertexShader;
}

ShaderName GraphicsPipelineManager::GetNoTransformVertexShader() const noexcept
{
	ShaderName vertexShader{};

	if (m_engineType == RenderEngineType::MeshDraw)
		vertexShader.SetName(L"MeshShaderMSIndividualNoTransform");
	else if (m_engineType == RenderEngineType::IndirectDraw)
		vertexShader.SetName(L"VertexShaderIndirectNoTransform");
	else if (m_engineType == RenderEngineType::IndividualDraw)
		vertexShader.SetName(L"VertexShaderIndividualNoTransform");

	return vertexShader;
}
