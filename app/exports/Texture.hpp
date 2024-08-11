#ifndef TEXTURE_SOL_HPP_
#define TEXTURE_SOL_HPP_
#include <memory>

struct STexture
{
	STexture() : data{}, width{ 0u }, height{ 0u } {}

	std::shared_ptr<void> data;
	std::uint32_t         width;
	std::uint32_t         height;

	STexture(const STexture&) = delete;
	STexture& operator=(const STexture&) = delete;

	STexture(STexture&& tex) noexcept
		: data{ std::move(tex.data) }, width{ tex.width }, height{ tex.height }
	{}

	STexture& operator=(STexture&& tex) noexcept
	{
		data   = std::move(tex.data);
		width  = tex.width;
		height = tex.height;

		return *this;
	}
};
#endif
