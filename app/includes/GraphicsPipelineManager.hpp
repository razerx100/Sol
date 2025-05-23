#ifndef GRAPHICS_PIPELINE_MANAGER_HPP_
#define GRAPHICS_PIPELINE_MANAGER_HPP_
#include <ExternalPipeline.hpp>
#include <ModuleTypes.hpp>

namespace Sol
{
namespace PSOStorage
{
	enum class ShaderType
	{
		OpaqueLight,
		TransparentLight,
		Count
	};

	void SetPipelineIndex(ShaderType type, std::uint32_t psoIndex) noexcept;

	[[nodiscard]]
	std::uint32_t GetPipelineIndex(ShaderType type) noexcept;
}

class GraphicsPipelineManager
{
public:
	GraphicsPipelineManager(RenderEngineType engineType);

	void SetMainPassOpaqueSignature(ExternalGraphicsPipeline&& pipelineSignature) noexcept;

	void SetTransparencyPassSignature(ExternalGraphicsPipeline&& pipelineSignature) noexcept;

	void SetTransparencyCombinePassSignature(ExternalGraphicsPipeline&& pipelineSignature) noexcept;

	[[nodiscard]]
	ShaderName GetDefaultVertexShader() const noexcept;
	[[nodiscard]]
	ShaderName GetNoTransformVertexShader() const noexcept;

	[[nodiscard]]
	const ExternalGraphicsPipeline& GetMainPassOpaqueSignature() const noexcept
	{
		return m_mainPassOpaqueSignature;
	}
	[[nodiscard]]
	const ExternalGraphicsPipeline& GetTransparencyPassSignature() const noexcept
	{
		return m_transparencyPassSignature;
	}
	[[nodiscard]]
	const ExternalGraphicsPipeline& GetTransparencyCombinePassSignature() const noexcept
	{
		return m_transparencyCombinePassSignature;
	}
	[[nodiscard]]
	bool IsMeshShaderPipeline() const noexcept
	{
		return m_engineType == RenderEngineType::MeshDraw;
	}

private:
	ExternalGraphicsPipeline m_mainPassOpaqueSignature;
	ExternalGraphicsPipeline m_transparencyPassSignature;
	ExternalGraphicsPipeline m_transparencyCombinePassSignature;
	RenderEngineType         m_engineType;

public:
	GraphicsPipelineManager(const GraphicsPipelineManager&) = delete;
	GraphicsPipelineManager& operator=(const GraphicsPipelineManager&) = delete;

	GraphicsPipelineManager(GraphicsPipelineManager&& other) noexcept
		: m_mainPassOpaqueSignature{ std::move(other.m_mainPassOpaqueSignature) },
		m_transparencyPassSignature{ std::move(other.m_transparencyPassSignature) },
		m_transparencyCombinePassSignature{ std::move(other.m_transparencyCombinePassSignature) },
		m_engineType{ other.m_engineType }
	{}
	GraphicsPipelineManager& operator=(GraphicsPipelineManager&& other) noexcept
	{
		m_mainPassOpaqueSignature          = std::move(other.m_mainPassOpaqueSignature);
		m_transparencyPassSignature        = std::move(other.m_transparencyPassSignature);
		m_transparencyCombinePassSignature = std::move(other.m_transparencyCombinePassSignature);
		m_engineType                       = other.m_engineType;

		return *this;
	}
};
}
#endif
