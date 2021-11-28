#ifndef __I_MODEL_HPP__
#define __I_MODEL_HPP__
#include <cstdint>

class IModel {
public:
	virtual ~IModel() = default;

	virtual void SetSolidColor(const float* const colorVector) noexcept = 0;

	virtual void GetVertices(
		float* vertices, std::uint32_t& vertexCount
	) const noexcept = 0;
	virtual void GetIndices(
		std::uint16_t* indices, std::uint32_t& indexCount
	) const noexcept = 0;

	virtual void GetTransform(float* matrix) const noexcept = 0;
	virtual void GetSolidColor(float* colorVector) const noexcept = 0;
};
#endif
