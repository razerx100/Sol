#ifndef LIGHT_SOURCE_HPP_
#define LIGHT_SOURCE_HPP_
#include <memory>
#include <ModelBase.hpp>

namespace Sol
{
class LightSource
{
public:
	LightSource(std::shared_ptr<Model> model) : m_model{ std::move(model) } {}
	// In case we don't want a rendered model.
	LightSource() : m_model{ std::make_shared<Model>() } {}

	void Move(const DirectX::XMFLOAT3& offset) noexcept
	{
		m_model->GetTransform().MoveModel(offset);
	}

	void MoveX(float offset) noexcept
	{
		m_model->GetTransform().MoveTowardsX(offset);
	}

	void MoveY(float offset) noexcept
	{
		m_model->GetTransform().MoveTowardsY(offset);
	}

	void MoveZ(float offset) noexcept
	{
		m_model->GetTransform().MoveTowardsZ(offset);
	}

	[[nodiscard]]
	DirectX::XMFLOAT3 GetPosition() const noexcept
	{
		return m_model->GetModelOffset();
	}

private:
	std::shared_ptr<Model> m_model;

public:
	LightSource(const LightSource&) = delete;
	LightSource& operator=(const LightSource&) = delete;

	LightSource(LightSource&& other) noexcept
		: m_model{ std::move(other.m_model) }
	{}
	LightSource& operator=(LightSource&& other) noexcept
	{
		m_model = std::move(other.m_model);

		return *this;
	}
};
}
#endif
