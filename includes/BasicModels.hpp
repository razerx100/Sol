#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <memory>
#include <concepts>

#include <Model.hpp>
#include <Sol.hpp>

class Triangle final : public Model {
public:
	Triangle(std::uint32_t indexCount) noexcept;

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
	Cube(std::uint32_t indexCount) noexcept;

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
	Quad(std::uint32_t indexCount) noexcept;

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

	ModelBoundingBox genericBoundingBox{};
	genericBoundingBox.Calculate(modelInputs->GetVertexData());

	std::vector<std::shared_ptr<Model>> models;
	for (size_t index = 0u; index < instances; ++index) {
		auto modelPtr = std::make_shared<model>(modelInputs->GetIndexCount());
		modelPtr->GetBoundingBox().SetBoundingCube(genericBoundingBox.GetBoundingCube());

		models.emplace_back(std::move(modelPtr));
	}

	Sol::modelContainer->AddModels(models, std::move(modelInputs));

	return models;
}
#endif
