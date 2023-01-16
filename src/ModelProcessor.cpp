#include <ModelProcessor.hpp>
#include <Sol.hpp>

// Model Processor
ModelProcessor::ModelProcessor() noexcept : m_indexOffset{ 0u } {}

bool ModelProcessor::FindModelInput(const std::string& inputName) const noexcept {
	if (auto inputType = m_modelInputTypes.find(inputName);
		inputType == std::end(m_modelInputTypes))
		return false;

	return true;
}

void ModelProcessor::AddModelInput(
	std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
) noexcept {
	modelInputs->InitData();

	ModelBoundingBox genericBoundingBox{};
	genericBoundingBox.Calculate(modelInputs->GetVertexData());

	InputData inputData{
		.indexCount = modelInputs->GetIndexCount(),
		.indexOffset = m_indexOffset,
		.boundingBox = genericBoundingBox
	};

	m_indexOffset += static_cast<std::uint32_t>(std::size(modelInputs->GetIndexData()));
	m_modelInputs.emplace_back(std::move(modelInputs));
	m_modelInputTypes.emplace(inputName, inputData);
}

void ModelProcessor::_addModel(
	std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	const InputData& inputData = m_modelInputTypes[inputName];
	model->SetIndexOffset(inputData.indexOffset);
	model->SetIndexCount(inputData.indexCount);
	model->SetBoundingBox(inputData.boundingBox);

	if (auto modelSet = m_modelSets.find(pixelShader); modelSet == std::end(m_modelSets))
		m_modelSets.emplace(pixelShader, std::vector<std::shared_ptr<IModel>>{});

	m_modelSets[pixelShader].emplace_back(std::move(model));
}

void ModelProcessor::Process() noexcept {
	std::uint32_t vertexOffset = 0u;
	size_t vertexBufferSize = 0u;
	size_t indexBufferSize = 0u;
	std::vector<std::uint32_t> gIndexBuffer;

	for (auto& modelInput : m_modelInputs) {
		for (std::uint32_t modelIndex : modelInput->GetIndexData())
			gIndexBuffer.emplace_back(vertexOffset + modelIndex);

		vertexOffset += static_cast<std::uint32_t>(std::size(modelInput->GetVertexData()));
		vertexBufferSize += modelInput->GetVertexBufferSize();
		indexBufferSize += modelInput->GetIndexBufferSize();
	}

	auto vertexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[vertexBufferSize]);
	auto indexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[indexBufferSize]);

	memcpy(indexBufferData.get(), std::data(gIndexBuffer), indexBufferSize);

	size_t vertexBufferOffset = 0u;
	for (auto& modelInput : m_modelInputs) {
		memcpy(
			vertexBufferData.get() + vertexBufferOffset, std::data(modelInput->GetVertexData()),
			modelInput->GetVertexBufferSize()
		);

		vertexBufferOffset += modelInput->GetVertexBufferSize();
	}

	Sol::renderer->SubmitModelInputs(
		std::move(vertexBufferData), vertexBufferSize,
		std::move(indexBufferData), indexBufferSize
	);

	for (auto& modelSet : m_modelSets)
		Sol::renderer->SubmitModelSet(std::move(modelSet.second), modelSet.first);
}
