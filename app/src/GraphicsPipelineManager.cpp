#include <GraphicsPipelineManager.hpp>

GraphicsPipelineManager::GraphicsPipelineManager()
	: m_mainPassOpaqueSignature{}, m_transparencyPassSignature{}, m_transparencyCombinePassSignature{}
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
