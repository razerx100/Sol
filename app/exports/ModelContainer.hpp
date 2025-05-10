#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <Model.hpp>
#include <ReusableVector.hpp>

class ModelContainer
{
public:
	ModelContainer() : m_models{} {}

	[[nodiscard]]
	std::uint32_t AddModel(Model&& model) noexcept
	{
		return static_cast<std::uint32_t>(m_models.Add(std::move(model)));
	}
	[[nodiscard]]
	std::vector<std::uint32_t> AddModels(std::vector<Model>&& models) noexcept
	{
		return m_models.AddElementsU32(std::move(models));
	}

	void RemoveModel(size_t index) noexcept
	{
		m_models.RemoveElement(index);
	}

	void RemoveModels(const std::vector<std::uint32_t>& indices) noexcept
	{
		for (size_t index : indices)
			m_models.RemoveElement(index);
	}

	[[nodiscard]]
	auto&& GetModels(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_models);
	}
	[[nodiscard]]
	auto&& GetModel(this auto&& self, size_t index) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_models[index]);
	}

	[[nodiscard]]
	size_t GetModelCount() const noexcept { return std::size(m_models); }

private:
	Callisto::ReusableVector<Model> m_models;

public:
	ModelContainer(const ModelContainer&) = delete;
	ModelContainer& operator=(const ModelContainer&) = delete;

	ModelContainer(ModelContainer&& other) noexcept
		: m_models{ std::move(other.m_models) }
	{}
	ModelContainer& operator=(ModelContainer&& other) noexcept
	{
		m_models = std::move(other.m_models);

		return *this;
	}
};
#endif
