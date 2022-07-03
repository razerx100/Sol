#include <ModelContainer.hpp>

#include <Sol.hpp>

void ModelContainer::AddModel(std::shared_ptr<Model> model) {
	m_models.emplace_back(std::move(model));
}

void ModelContainer::ClearModelBuffers() noexcept {
	for (auto& model : m_models)
		model->ResetVerticesAndIndices();
}

void ModelContainer::SubmitModelsToRenderer() const noexcept {
	for (auto& model : m_models)
		Sol::renderer->SubmitModel(model);
}
