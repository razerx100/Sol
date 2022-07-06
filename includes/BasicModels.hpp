#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <memory>
#include <concepts>

#include <Model.hpp>
#include <Sol.hpp>

class Triangle final : public Model {
public:
	Triangle() noexcept;

private:
	void Init() noexcept;
};

class TriangleInputs final : public ModelInputs {
public:
	TriangleInputs() noexcept;

private:
	void Init() noexcept;
};

class Cube final : public Model {
public:
	Cube() noexcept;

private:
	void Init() noexcept;
};

class CubeInputs final : public ModelInputs {
public:
	CubeInputs() noexcept;

private:
	void Init() noexcept;
};

class Quad final : public Model {
public:
	Quad() noexcept;

private:
	void Init() noexcept;
};

class QuadInputs final : public ModelInputs {
public:
	QuadInputs() noexcept;

private:
	void Init() noexcept;
};

// Factory function
template<class model, class input>
	requires std::derived_from<model, Model> && std::derived_from<input, ModelInputs>
std::vector<std::shared_ptr<Model>> AddAndGetModels(std::uint32_t instances) noexcept {
	auto modelInputs = std::make_unique<input>();

	std::vector<std::shared_ptr<Model>> models;
	for (size_t index = 0u; index < instances; ++index) {
		models.emplace_back(std::make_shared<model>());
		models.back()->SetIndexCount(modelInputs->GetIndexCount());
	}

	Sol::modelContainer->AddModels(models, std::move(modelInputs));

	return models;
}
#endif
