#ifndef MODEL_SOL_HPP_
#define MODEL_SOL_HPP_
#include <cstdint>
#include <memory>
#include <vector>

#include <DirectXMath.h>

struct UVInfo
{
	float uOffset = 0.f;
	float vOffset = 0.f;
	float uScale  = 1.f;
	float vScale  = 1.f;
};

// Represent a single drawable object.
class Model
{
public:
	virtual ~Model() = default;

	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetModelOffset() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetMaterialIndex() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetMeshIndex() const noexcept = 0;
	[[nodiscard]]
	virtual float GetModelScale() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsVisible() const noexcept = 0;

	// This will be set from the renderers. Didn't want to set it from the consuming classes
	// but since every renderer will have model buffer index, this would be better.
	virtual void SetModelIndexInBuffer(std::uint32_t index) noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetModelIndexInBuffer() const noexcept = 0;

	[[nodiscard]]
	virtual std::uint32_t GetDiffuseIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetDiffuseUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetSpecularIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetSpecularUVInfo() const noexcept = 0;
};

// Should contain all the models of a Model Bundle which have a certain pipeline.
class PipelineModelBundle
{
public:
	virtual ~PipelineModelBundle() = default;

	[[nodiscard]]
	virtual std::uint32_t GetPipelineIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetModelIndicesInBundle() const noexcept = 0;
};

// Should typically have a complex model with multiple models.
class ModelBundle
{
public:
	using ModelContainer_t    = std::vector<std::shared_ptr<Model>>;
	using PipelineContainer_t = std::vector<std::shared_ptr<PipelineModelBundle>>;

public:
	virtual ~ModelBundle() = default;

	[[nodiscard]]
	virtual std::uint32_t GetMeshBundleIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const PipelineContainer_t& GetPipelineBundles() const noexcept = 0;
	[[nodiscard]]
	virtual const ModelContainer_t& GetModels() const noexcept = 0;
	[[nodiscard]]
	virtual ModelContainer_t& GetModels() noexcept = 0;
};
#endif
