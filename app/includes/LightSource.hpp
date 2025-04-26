#ifndef LIGHT_SOURCE_HPP_
#define LIGHT_SOURCE_HPP_
#include <memory>
#include <ModelBase.hpp>

namespace Sol
{
class LightSource
{
public:
	virtual ~LightSource() = default;

	virtual void Move(const DirectX::XMFLOAT3& offset) noexcept = 0;

	virtual void MoveX(float offset) noexcept = 0;
	virtual void MoveY(float offset) noexcept = 0;
	virtual void MoveZ(float offset) noexcept = 0;

	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetPosition() const noexcept = 0;
};

class LightSourceWithoutModel : public LightSource
{
public:
	LightSourceWithoutModel() : m_position{ 0.f, 0.f, 0.f } {}
	LightSourceWithoutModel(const DirectX::XMFLOAT3& position) : m_position{ position } {}

	void Move(const DirectX::XMFLOAT3& offset) noexcept
	{
		// I don't think a SIMD addition here is worth it. Since, that would be
		// 2 loads + 1 addition and then 1 store.
		m_position.x += offset.x;
		m_position.y += offset.y;
		m_position.z += offset.z;
	}

	void MoveX(float offset) noexcept override
	{
		m_position.x += offset;
	}

	void MoveY(float offset) noexcept override
	{
		m_position.y += offset;
	}

	void MoveZ(float offset) noexcept override
	{
		m_position.z += offset;
	}

	[[nodiscard]]
	DirectX::XMFLOAT3 GetPosition() const noexcept override { return m_position; }

private:
	DirectX::XMFLOAT3 m_position;

public:
	// We shouldn't be able to duplicate a light source. So, artifical enforcement.
	LightSourceWithoutModel(const LightSourceWithoutModel&) = delete;
	LightSourceWithoutModel& operator=(const LightSourceWithoutModel&) = delete;

	LightSourceWithoutModel(LightSourceWithoutModel&& other) noexcept
		: m_position{ other.m_position }
	{}
	LightSourceWithoutModel& operator=(LightSourceWithoutModel&& other) noexcept
	{
		m_position = other.m_position;

		return *this;
	}
};

class LightSourceWithModel : public LightSource
{
public:
	LightSourceWithModel(std::shared_ptr<ModelBase> model) : m_model{ std::move(model) } {}

	void Move(const DirectX::XMFLOAT3& offset) noexcept
	{
		m_model->GetTransform().MoveModel(offset);
	}

	void MoveX(float offset) noexcept override
	{
		m_model->GetTransform().MoveTowardsX(offset);
	}

	void MoveY(float offset) noexcept override
	{
		m_model->GetTransform().MoveTowardsY(offset);
	}

	void MoveZ(float offset) noexcept override
	{
		m_model->GetTransform().MoveTowardsZ(offset);
	}

	[[nodiscard]]
	DirectX::XMFLOAT3 GetPosition() const noexcept override
	{
		return m_model->GetModelOffset();
	}

private:
	std::shared_ptr<ModelBase> m_model;

public:
	LightSourceWithModel(const LightSourceWithModel&) = delete;
	LightSourceWithModel& operator=(const LightSourceWithModel&) = delete;

	LightSourceWithModel(LightSourceWithModel&& other) noexcept
		: m_model{ std::move(other.m_model) }
	{}
	LightSourceWithModel& operator=(LightSourceWithModel&& other) noexcept
	{
		m_model = std::move(other.m_model);

		return *this;
	}
};
}
#endif
