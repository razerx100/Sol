#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <memory>
#include <vector>

#include <Model.hpp>

class ModelContainer {
public:
	void AddModels(
		std::vector<std::shared_ptr<Model>> models, std::unique_ptr<ModelInputs> modelInputs
	) noexcept;

	void SubmitModelsToRenderer() noexcept;

private:
	std::vector<std::unique_ptr<ModelInputs>> m_modelInputs;
	std::vector<std::vector<std::shared_ptr<Model>>> m_models;
};
#endif
