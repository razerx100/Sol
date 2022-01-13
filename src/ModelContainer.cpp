#include <ModelContainer.hpp>

ISolModel* ModelContainer::AddModel(std::unique_ptr<ISolModel> model) {
	m_models.emplace_back(std::move(model));

	return m_models.back().get();
}

void ModelContainer::ClearModelBuffers() noexcept {
	for (auto& model : m_models)
		model->ResetVerticesAndIndices();
}
