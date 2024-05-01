#include <ColourTexture.hpp>

ColourTexture::ColourTexture() noexcept
	: m_16bitsComponent(false) {}

ColourTexture& ColourTexture::AddColour(
	const std::string& name, const DirectX::XMVECTORF32& colour
) noexcept {
	m_colourNames.emplace_back(name);

	DirectX::XMFLOAT4 colourF4 = {};
	DirectX::XMStoreFloat4(&colourF4, colour);

	if (m_16bitsComponent)
		m_unprocessedColourU16.emplace_back(ColourCast<std::uint16_t>(colourF4));
	else
		m_unprocessedColourU8.emplace_back(ColourCast<std::uint8_t>(colourF4));

	return *this;
}

void ColourTexture::CreateTexture() noexcept {
	if (m_16bitsComponent) {
		size_t textureSize = sizeof(RGBA16) * (std::size(m_unprocessedColourU16));
		m_texture = std::unique_ptr<std::uint8_t>(new std::uint8_t[textureSize]);

		MemCopyToTexture(m_unprocessedColourU16);
	}
	else {
		size_t textureSize = sizeof(RGBA8) * (std::size(m_unprocessedColourU8));
		m_texture = std::unique_ptr<std::uint8_t>(new std::uint8_t[textureSize]);

		MemCopyToTexture(m_unprocessedColourU8);
	}
}

std::unique_ptr<std::uint8_t> ColourTexture::MoveTexture() noexcept {
	return std::move(m_texture);
}

const std::vector<std::string>& ColourTexture::GetNames() const noexcept {
	return m_colourNames;
}

std::uint32_t ColourTexture::GetWidth() const noexcept {
	return static_cast<std::uint32_t>(std::size(m_colourNames));
}

void ColourTexture::SetIfComponentsAre16bits(bool component16bits) noexcept {
	m_16bitsComponent = component16bits;
}

bool ColourTexture::IsThereAnyColours() const noexcept {
	return !std::empty(m_colourNames);
}
