#include <ModelContainer.hpp>
#include <InstanceManager.hpp>

ISolModel* ModelContainer::AddModel(std::unique_ptr<ISolModel> model) {
	m_models.emplace_back(std::move(model));

	return m_models.back().get();
}

void ModelContainer::ClearModelBuffers() noexcept {
	for (auto& model : m_models)
		model->ResetVerticesAndIndices();
}

void ModelContainer::UpdateUVCoordinates() noexcept {
	//ITextureAtlas* texRef = TexAtlasInst::GetRef();
	//size_t atlasWidth = texRef->GetWidth();
	//size_t atlasHeight = texRef->GetHeight();

		// Complex textures will need more work
}
