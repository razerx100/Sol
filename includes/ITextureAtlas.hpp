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
typedef _UV<std::uint32_t> UVU32;

class ITextureAtlas {
public:
	virtual ~ITextureAtlas() = default;

	virtual void AddColour(
		const std::string& name, const Ceres::Float32_4& colour
	) noexcept = 0;
	virtual void AddTexture(
		const std::string& name, const std::vector<std::uint8_t>& data,
		size_t width, size_t height, size_t pixelSizeInBytes
	) noexcept = 0;
	virtual void CreateAtlas() noexcept = 0;
	virtual void CleanUpBuffer() noexcept = 0;

	virtual UVU32 GetPixelData(const std::string& name) const noexcept = 0;

	virtual std::uint32_t GetWidth() const noexcept = 0;
	virtual std::uint32_t GetHeight() const noexcept = 0;
	virtual size_t GetPixelSizeInBytes() const noexcept = 0;

	virtual const std::vector<std::uint8_t>& GetTexture() const noexcept = 0;
};

ITextureAtlas* CreateTextureAtlasInstance();

//constexpr float PixelToUV(size_t pixelCoord, size_t maxLength) noexcept {
//	return static_cast<float>((pixelCoord - 1) * 2 + 1) / (maxLength * 2);
//}
#endif
