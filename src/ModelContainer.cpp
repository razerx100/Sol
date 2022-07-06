#include <ModelContainer.hpp>
#include <Sol.hpp>

void ModelContainer::AddModels(
	std::vector<std::shared_ptr<Model>> models, std::unique_ptr<ModelInputs> modelInputs
) noexcept {
	m_modelInputs.emplace_back(std::move(modelInputs));
	m_models.emplace_back(std::move(models));
}

void ModelContainer::SubmitModelsToRenderer() noexcept {
	for (size_t index = 0u; index < std::size(m_modelInputs); ++index) {
		std::vector<std::shared_ptr<IModel>> iModels;
		for (auto& model : m_models[index])
			iModels.emplace_back(model);

		Sol::renderer->SubmitModels(std::move(iModels), std::move(m_modelInputs[index]));
	}
}
