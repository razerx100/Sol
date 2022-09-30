#ifndef COLOUR_TEXTURE_HPP_
#define COLOUR_TEXTURE_HPP_
#include <vector>
#include <memory>
#include <string>

#include <DirectXMath.h>

template<typename T>
struct _RGBA {
	_RGBA() = default;
	constexpr _RGBA(T _r, T _g, T _b, T _a) noexcept : r(_r), g(_g), b(_b), a(_a) {}

	T r;
	T g;
	T b;
	T a;
};

using RGBA8 = _RGBA<std::uint8_t>;
using RGBA16 = _RGBA<std::uint16_t>;

class ColourTexture {
public:
	ColourTexture() noexcept;

	void AddColour(const std::string& name, const DirectX::XMVECTORF32& colour) noexcept;
	void CreateTexture() noexcept;

	void SetIfComponentsAre16bits(bool component16bits) noexcept;

	[[nodiscard]]
	std::uint32_t GetWidth() const noexcept;

	[[nodiscard]]
	std::unique_ptr<std::uint8_t> MoveTexture() noexcept;
	[[nodiscard]]
	const std::vector<std::string>& GetNames() const noexcept;
	[[nodiscard]]
	bool IsThereAnyColours() const noexcept;

private:
	std::vector<std::string> m_colourNames;
	std::vector<RGBA16> m_unprocessedColourU16;
	std::vector<RGBA8> m_unprocessedColourU8;
	std::unique_ptr<std::uint8_t> m_texture;
	bool m_16bitsComponent;

public:
	template<std::integral From>
	void AddColour(const std::string& name, const _RGBA<From>& colour) noexcept {
		m_colourNames.emplace_back(name);

		if (m_16bitsComponent)
			m_unprocessedColourU16.emplace_back(ColourCast<std::uint16_t>(colour));
		else
			m_unprocessedColourU8.emplace_back(ColourCast<std::uint8_t>(colour));
	}

private:
	template<std::integral T>
	void MemCopyToTexture(std::vector<_RGBA<T>>& unproccessedColour) noexcept {
		size_t bytesPerPixel = m_16bitsComponent ? 8u : 4u;

		for (size_t index = 0u; index < std::size(unproccessedColour); ++index) {
			size_t colourSize = sizeof(T) * bytesPerPixel;

			std::memcpy(
				m_texture.get() + (index * colourSize),
				&unproccessedColour[index], colourSize
			);
		}

		unproccessedColour = std::vector<_RGBA<T>>();
	}

	template<std::integral To>
	_RGBA<To> ColourCast(const DirectX::XMFLOAT4& colour) noexcept {
		constexpr size_t multiplier = std::numeric_limits<To>::max();

		_RGBA<To> transformedColour = {};
		transformedColour.r = static_cast<To>(colour.x * multiplier);
		transformedColour.g = static_cast<To>(colour.y * multiplier);
		transformedColour.b = static_cast<To>(colour.z * multiplier);
		transformedColour.a = static_cast<To>(colour.w * multiplier);

		return transformedColour;
	}

	template<std::integral To, std::integral From>
	_RGBA<To> ColourCast(const _RGBA<From>& colour) noexcept {
		constexpr size_t multiplier = std::numeric_limits<To>::max();
		constexpr size_t divisor = std::numeric_limits<From>::max();

		if constexpr (multiplier == divisor)
			return colour;
		else {
			_RGBA<To> transformedColour = {};
			transformedColour.r =
				static_cast<To>(static_cast<float>(colour.r) / divisor * multiplier);
			transformedColour.g =
				static_cast<To>(static_cast<float>(colour.g) / divisor * multiplier);
			transformedColour.b =
				static_cast<To>(static_cast<float>(colour.b) / divisor * multiplier);
			transformedColour.a =
				static_cast<To>(static_cast<float>(colour.a) / divisor * multiplier);

			return transformedColour;
		}
	}
};
#endif
