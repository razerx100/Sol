#ifndef __I_TEXTURE_ATLAS_HPP__
#define __I_TEXTURE_ATLAS_HPP__
#include <cstdint>
#include <string>
#include <optional>
#include <CRSColour.hpp>
#include <vector>

template<typename T>
struct _UV {
	T uStart;
	T uEnd;
	T vStart;
	T vEnd;
};

typedef _UV<float> UVF;
typedef _UV<size_t> UVU;

class ITextureAtlas {
public:
	virtual ~ITextureAtlas() = default;

	virtual void AddColour(
		const std::string& name, const Ceres::Float32_4& colour
	) noexcept = 0;
	virtual void AddTexture(
		const std::string& name, const std::vector<std::uint8_t>& data,
		size_t rowPitch, size_t rows
	) noexcept = 0;
	virtual void CreateAtlas() noexcept = 0;

	virtual std::optional<UVF> GetUVData(const std::string& name) const noexcept = 0;

	virtual size_t GetRowPitch() const noexcept = 0;
	virtual size_t GetRows() const noexcept = 0;
	virtual const std::vector<std::uint8_t>& GetTexture() const noexcept = 0;
};

ITextureAtlas* CreateTextureAtlasInstance();
#endif
