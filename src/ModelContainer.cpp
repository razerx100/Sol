#include <ModelContainer.hpp>

void ModelContainer::AddModel(std::shared_ptr<Model> model) {
	m_models.emplace_back(std::move(model));
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
