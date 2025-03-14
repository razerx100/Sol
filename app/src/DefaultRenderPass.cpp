#include <DefaultRenderPass.hpp>

void DefaultRenderPass::SetRenderTarget(ExternalResourceFactory& resourceFactory)
{
	m_renderTargetIndex = static_cast<std::uint32_t>(resourceFactory.CreateExternalTexture());

	m_renderTarget      = resourceFactory.GetExternalTextureSP(m_renderTargetIndex);
}

void DefaultRenderPass::SetDepthBuffer(ExternalResourceFactory& resourceFactory)
{
	m_depthBufferIndex = static_cast<std::uint32_t>(resourceFactory.CreateExternalTexture());

	m_depthBuffer      = resourceFactory.GetExternalTextureSP(m_depthBufferIndex);
}

void DefaultRenderPass::FetchRenderPassFromRenderer()
{
	m_renderer->SetSwapchainExternalRenderPass();

	m_renderPass = m_renderer->GetSwapchainExternalRenderPassSP();

	const std::uint32_t renderTargetIndex = m_renderPass->AddRenderTarget(
		m_renderTargetIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_renderPass->SetRenderTargetClearColour(
		renderTargetIndex, DirectX::XMFLOAT4{ 0.005f, 0.005f, 0.005f, 1.f }
	);

	m_renderPass->SetDepthTesting(
		m_depthBufferIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::DontCare
	);

	m_renderPass->SetSwapchainCopySource(renderTargetIndex);
}

void DefaultRenderPass::Resize()
{
	m_renderer->WaitForGPUToFinish();

	Renderer::Extent renderArea = m_renderer->GetCurrentRenderingExtent();

	m_renderTarget->Create(
		renderArea.width, renderArea.height, m_renderer->GetSwapchainFormat(),
		ExternalTexture2DType::RenderTarget, true, false
	);

	m_depthBuffer->Create(
		renderArea.width, renderArea.height, ExternalFormat::D32_FLOAT, ExternalTexture2DType::Depth,
		false, false
	);

	m_renderPass->ResetAttachmentReferences();
}

void DefaultRenderPass::ResizeCallback([[maybe_unused]] void* callbackData, void* extraData)
{
	auto sThis = static_cast<DefaultRenderPass*>(extraData);

	sThis->Resize();
}
