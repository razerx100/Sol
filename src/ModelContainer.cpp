#include <ModelContainer.hpp>
#include <Sol.hpp>

void ModelContainer::SetResources() {
	for (auto& model : m_models)
		model->SetResources();
}

void ModelContainer::PhysicsUpdate() noexcept {
	for (auto& model : m_models)
		model->PhysicsUpdate();
}

ModelProcessor* ModelContainer::GetModelProcessor() const noexcept {
	return Sol::modelProcessor.get();
}
