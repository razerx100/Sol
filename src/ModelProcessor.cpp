#include <ModelProcessor.hpp>
#include <ranges>
#include <algorithm>

#include <Sol.hpp>

// Model Processor
ModelProcessor::ModelProcessor(const Args& arguments) noexcept
	: m_processingModeMeshlet{ arguments.processingModeMeshlet.value() } {}

bool ModelProcessor::FindModelInput(const std::string& inputName) const noexcept {
	bool result = true;

	if (m_processingModeMeshlet) {
		if (auto inputType = m_modelDataMeshlets.find(inputName);
			inputType == std::end(m_modelDataMeshlets))
			result = false;
	}
	else
		if (auto inputType = m_modelData.find(inputName); inputType == std::end(m_modelData))
			result = false;

	return result;
}

void ModelProcessor::AddToGVertices(const std::vector<Vertex>& vertices) noexcept {
	std::ranges::copy(vertices, std::back_inserter(m_gVertices));
}

void ModelProcessor::AddModelData(
	std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
) noexcept {
	ModelBoundingBox genericBoundingBox{};
	genericBoundingBox.Calculate(modelInputs->GetVertexData());

	IndexData modelData{
		.indexCount = modelInputs->GetIndexCount(),
		.indexOffset = static_cast<std::uint32_t>(std::size(m_gIndices)),
		.boundingBox = genericBoundingBox
	};

	const std::uint32_t indexOffset = static_cast<std::uint32_t>(std::size(m_gVertices));
	for (auto index : modelInputs->GetIndexData())
		m_gIndices.emplace_back(indexOffset + index);

	AddToGVertices(modelInputs->GetVertexData());

	m_modelData.emplace(inputName, modelData);
}

void ModelProcessor::AddModelDataMeshlet(
	std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
) noexcept {

}

void ModelProcessor::AddModelInput(
	std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
) noexcept {
	modelInputs->InitData();

	if (m_processingModeMeshlet)
		AddModelDataMeshlet(std::move(modelInputs), inputName);
	else
		AddModelData(std::move(modelInputs), inputName);
}

void ModelProcessor::_addModelModelData(
	std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	const IndexData& modelData = m_modelData[inputName];
	model->SetIndexOffset(modelData.indexOffset);
	model->SetIndexCount(modelData.indexCount);
	model->SetBoundingBox(modelData.boundingBox);

	if (auto modelSet = m_modelSets.find(pixelShader); modelSet == std::end(m_modelSets))
		m_modelSets.emplace(pixelShader, std::vector<std::shared_ptr<IModel>>{});

	m_modelSets[pixelShader].emplace_back(std::move(model));
}

void ModelProcessor::_addModelMeshlets(
	std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
) noexcept {

}

void ModelProcessor::_addModel(
	std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	if (m_processingModeMeshlet)
		_addModelMeshlets(std::move(model), inputName, pixelShader);
	else
		_addModelModelData(std::move(model), inputName, pixelShader);
}

void ModelProcessor::Process() noexcept {
	const size_t vertexBufferSize = sizeof(Vertex) * std::size(m_gVertices);
	const size_t indexBufferSize = sizeof(std::uint32_t) * std::size(m_gIndices);

	auto vertexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[vertexBufferSize]);
	auto indexBufferData = std::unique_ptr<std::uint8_t>(new std::uint8_t[indexBufferSize]);

	memcpy(indexBufferData.get(), std::data(m_gIndices), indexBufferSize);
	memcpy(vertexBufferData.get(), std::data(m_gVertices), vertexBufferSize);

	Sol::renderer->AddModelInputs(
		std::move(vertexBufferData), vertexBufferSize,
		std::move(indexBufferData), indexBufferSize
	);

	for (auto& modelSet : m_modelSets)
		Sol::renderer->AddModelSet(std::move(modelSet.second), modelSet.first);
}
