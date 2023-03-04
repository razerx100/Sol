#ifndef I_MODEL_HPP_
#define I_MODEL_HPP_
#include <cstdint>
#include <memory>
#include <vector>

#include <DirectXMath.h>

struct UVInfo {
	float uOffset;
	float vOffset;
	float uRatio;
	float vRatio;
};

struct ModelBounds {
	DirectX::XMFLOAT3 positiveAxes;
	DirectX::XMFLOAT3 negativeAxes;
};

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct Material {
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	float shininess = 1.f;
};

class IModel {
public:
	virtual ~IModel() = default;

	[[nodiscard]]
	virtual std::uint32_t GetIndexCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetIndexOffset() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetDiffuseTexIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetDiffuseTexUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetSpecularTexIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetSpecularTexUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetModelOffset() const noexcept = 0;
	[[nodiscard]]
	virtual ModelBounds GetBoundingBox() const noexcept = 0;
	[[nodiscard]]
	virtual Material GetMaterial() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsLightSource() const noexcept = 0;
};

struct Meshlet {
	std::uint32_t vertexCount;
	std::uint32_t vertexOffset;
	std::uint32_t primitiveCount;
	std::uint32_t primitiveOffset;
};

struct MeshletModel {
	MeshletModel() = default;
	MeshletModel(const MeshletModel&) = delete;

	inline MeshletModel(std::shared_ptr<IModel> model, std::vector<Meshlet> meshlets) noexcept
		: model{ std::move(model) }, meshlets{ std::move(meshlets) } {}
	inline MeshletModel(MeshletModel&& other) noexcept
		: model(std::move(other.model)), meshlets(std::move(other.meshlets)) {}

	MeshletModel& operator=(const MeshletModel&) = delete;
	inline MeshletModel& operator=(MeshletModel&& other) noexcept {
		model = std::move(other.model);
		meshlets = std::move(other.meshlets);

		return *this;
	}

	std::shared_ptr<IModel> model;
	std::vector<Meshlet> meshlets;
};
#endif
