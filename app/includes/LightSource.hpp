#ifndef LIGHT_SOURCE_HPP_
#define LIGHT_SOURCE_HPP_
#include <memory>
#include <limits>

#include <DirectXMath.h>

namespace Sol
{
class LightSource
{
public:
	LightSource(std::uint32_t modelIndexInContainer)
		: m_position{ 0.f, 0.f, 0.f }, m_modelIndexInContainer{ modelIndexInContainer }
	{}
	// In case we don't want a rendered model.
	LightSource()
		: m_position{ 0.f, 0.f, 0.f },
		m_modelIndexInContainer{ std::numeric_limits<std::uint32_t>::max() }
	{}

	void Move(const DirectX::XMFLOAT3& offset) noexcept
	{
		m_position = offset;
	}

	void MoveX(float offset) noexcept
	{
		m_position.x += offset;
	}

	void MoveY(float offset) noexcept
	{
		m_position.y += offset;
	}

	void MoveZ(float offset) noexcept
	{
		m_position.z += offset;
	}

	[[nodiscard]]
	DirectX::XMFLOAT3 GetPosition() const noexcept
	{
		return m_position;
	}

	[[nodiscard]]
	bool HasModel() const noexcept
	{
		return m_modelIndexInContainer != std::numeric_limits<std::uint32_t>::max();
	}

	[[nodiscard]]
	std::uint32_t GetModelIndexInContainer() const noexcept { return m_modelIndexInContainer; }

private:
	DirectX::XMFLOAT3 m_position;
	std::uint32_t     m_modelIndexInContainer;

public:
	LightSource(const LightSource&) = delete;
	LightSource& operator=(const LightSource&) = delete;

	LightSource(LightSource&& other) noexcept
		: m_position{ other.m_position },
		m_modelIndexInContainer{ other.m_modelIndexInContainer }
	{}
	LightSource& operator=(LightSource&& other) noexcept
	{
		m_position              = other.m_position;
		m_modelIndexInContainer = other.m_modelIndexInContainer;

		return *this;
	}
};
}
#endif
