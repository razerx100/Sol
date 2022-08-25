#include <ModelContainer.hpp>
#include <Sol.hpp>

void ModelContainer::AddModels(
	std::vector<std::shared_ptr<Model>> models, std::unique_ptr<ModelInputs> modelInputs
) noexcept {
	m_modelInputs.emplace_back(std::move(modelInputs));
	m_models.emplace_back(std::move(models));
}

void ModelContainer::SubmitModelsToRenderer() noexcept {
	size_t vertexBufferSize = 0u;
	size_t vertexStrideSize = sizeof(Vertex);
	size_t indexBufferSize = 0u;
	std::uint32_t indexOffset = 0u;
	std::uint32_t vertexOffset = 0u;

	std::vector<std::uint32_t> gIndexBuffer;
	for (size_t index = 0u; index < std::size(m_modelInputs); ++index) {
		std::vector<std::shared_ptr<IModel>> iModels;
		for (auto& model : m_models[index]) {
			model->SetIndexOffset(indexOffset);
			iModels.emplace_back(model);
		}

		Sol::renderer->SubmitModels(std::move(iModels));

		std::unique_ptr<ModelInputs>& modelInput = m_modelInputs[index];

		vertexBufferSize += modelInput->GetVertexBufferSize();
		indexBufferSize += modelInput->GetIndexBufferSize();

		for (std::uint32_t modelIndex : modelInput->GetIndexData())
			gIndexBuffer.emplace_back(vertexOffset + modelIndex);

		indexOffset += static_cast<std::uint32_t>(std::size(modelInput->GetIndexData()));
		vertexOffset += static_cast<std::uint32_t>(std::size(modelInput->GetVertexData()));
	}

	auto vertexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[vertexBufferSize]);
	auto indexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[indexBufferSize]);

	memcpy(indexBufferData.get(), std::data(gIndexBuffer), indexBufferSize);

	size_t vertexBufferOffset = 0u;
	for (auto& modelInput : m_modelInputs) {
		memcpy(
			vertexBufferData.get() + vertexBufferOffset,
			std::data(modelInput->GetVertexData()),
			modelInput->GetVertexBufferSize()
		);

		vertexBufferOffset += modelInput->GetVertexBufferSize();
	}

	Sol::renderer->SubmitModelInputs(
		std::move(vertexBufferData), vertexBufferSize, vertexStrideSize,
		std::move(indexBufferData), indexBufferSize
	);
}
