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
	ITextureAtlas* texRef = TexAtlasInst::GetRef();

	for (auto& model : m_models) {
		auto uvf = texRef->GetUVData(model->GetTextureName());

		if (uvf)
			model->UpdateUV(uvf->uStart, uvf->vStart);
	}
}
