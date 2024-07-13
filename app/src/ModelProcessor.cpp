#include <ModelProcessor.hpp>
#include <ranges>
#include <algorithm>

#include <Sol.hpp>

/*
// Model Processor
ModelProcessor::ModelProcessor(const Args& arguments) noexcept
	: m_processingModeMeshlet{ arguments.processingModeMeshlet.value() },
	m_meshletVertexLimit{ 64u }, m_meshletPrimitiveLimit{ 126u } {}

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
	//ModelBoundingBox genericBoundingBox{};
	//genericBoundingBox.Calculate(modelInputs->GetVertexData());

	IndexData modelData{
		.indexCount = modelInputs->GetIndexCount(),
		.indexOffset = static_cast<std::uint32_t>(std::size(m_gIndices)),
		//.boundingBox = genericBoundingBox
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
	const size_t indexCountLimit = static_cast<size_t>(m_meshletPrimitiveLimit) * 3u;
	const auto gVertexCount = static_cast<std::uint32_t>(std::size(m_gVertices));
	const std::vector<std::uint32_t>& indices = modelInputs.get()->GetIndexData();
	const std::vector<Vertex>& vertices = modelInputs.get()->GetVertexData();

	std::vector<Meshlet> meshlets;
	for (size_t start = 0u; start < std::size(indices);)
		meshlets.emplace_back(MakeMeshlet(indices, start, indexCountLimit, gVertexCount));

	AddToGVertices(vertices);

	m_modelDataMeshlets.emplace(inputName, std::move(meshlets));
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
	std::shared_ptr<ModelBase> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	if (auto modelSet = m_modelSets.find(pixelShader); modelSet == std::end(m_modelSets))
		m_modelSets.emplace(pixelShader, std::vector<std::shared_ptr<Model>>{});

	const IndexData& modelData = m_modelData[inputName];
	model->SetIndexOffset(modelData.indexOffset);
	model->SetIndexCount(modelData.indexCount);
	model->SetBoundingBox(modelData.boundingBox);

	m_modelSets[pixelShader].emplace_back(std::move(model));
}

void ModelProcessor::_addModelMeshlets(
	std::shared_ptr<ModelBase> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	if (auto modelSet = m_meshletModelSets.find(pixelShader);
		modelSet == std::end(m_meshletModelSets))
		m_meshletModelSets.emplace(pixelShader, std::vector<MeshletModel>{});

	const std::vector<Meshlet>& meshlets = m_modelDataMeshlets[inputName];

	MeshletModel meshModel{ std::move(model), meshlets };

	m_meshletModelSets[pixelShader].emplace_back(std::move(meshModel));
}

void ModelProcessor::_addModel(
	std::shared_ptr<ModelBase> model, const std::string& inputName, std::wstring pixelShader
) noexcept {
	if (m_processingModeMeshlet)
		_addModelMeshlets(std::move(model), inputName, pixelShader);
	else
		_addModelModelData(std::move(model), inputName, pixelShader);
}

void ModelProcessor::_moveDataModels() noexcept {
	Sol::renderer->AddModelInputs(std::move(m_gVertices), std::move(m_gIndices));

	for (auto& modelSet : m_modelSets)
		Sol::renderer->AddModelSet(std::move(modelSet.second), modelSet.first);
}

void ModelProcessor::_moveDataMeshletModels() noexcept {
	Sol::renderer->AddModelInputs(
		std::move(m_gVertices), std::move(m_gVerticesIndices), std::move(m_gIndices)
	);

	for (auto& modelSet : m_meshletModelSets)
		Sol::renderer->AddMeshletModelSet(std::move(modelSet.second), modelSet.first);
}

void ModelProcessor::MoveData() noexcept {
	if (m_processingModeMeshlet)
		_moveDataMeshletModels();
	else
		_moveDataModels();
}

void ModelProcessor::SetMeshletVertexAndPrimitiveLimits(
	std::uint32_t vertexLimit, std::uint32_t primitiveLimit
) noexcept {
	m_meshletVertexLimit = vertexLimit;
	m_meshletPrimitiveLimit = primitiveLimit;
}

Meshlet ModelProcessor::MakeMeshlet(
	const std::vector<std::uint32_t>& indices, size_t& startingIndex, size_t indexCount,
	std::uint32_t vertexOffset
) noexcept {
	const size_t meshletIndexLimit = startingIndex + indexCount;
	std::unordered_map<std::uint32_t, std::uint32_t> vertexIndicesM;
	std::vector<std::uint32_t> vertexIndices;
	std::vector<std::uint32_t> primIndices;

	size_t& indexOffset = startingIndex;
	for (; indexOffset < std::size(indices) && indexOffset < meshletIndexLimit;
		indexOffset += 3u) {
		const std::uint32_t vIndex1 = indices[indexOffset] + vertexOffset;
		const std::uint32_t vIndex2 = indices[indexOffset + 1u] + vertexOffset;
		const std::uint32_t vIndex3 = indices[indexOffset + 2u] + vertexOffset;

		const auto vertexCount = static_cast<std::uint32_t>(std::size(vertexIndices));
		const std::uint32_t couldBeVertexCount = vertexCount + GetExtraVertexCount(
			vertexIndicesM, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVertexCount > m_meshletVertexLimit)
			break;

		const std::uint32_t primIndexOne = GetPrimIndex(vIndex1, vertexIndicesM, vertexIndices);
		const std::uint32_t primIndexTwo = GetPrimIndex(vIndex2, vertexIndicesM, vertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(
			vIndex3, vertexIndicesM, vertexIndices
		);

		primIndices.emplace_back(
			PrimitiveIndices{ .unpacked = {primIndexOne, primIndexTwo, primIndexThree} }.packed
		);
	}

	Meshlet meshlet{
		.vertexCount = static_cast<std::uint32_t>(std::size(vertexIndices)),
		.vertexOffset = static_cast<std::uint32_t>(std::size(m_gVerticesIndices)),
		.primitiveCount = static_cast<std::uint32_t>(std::size(primIndices)),
		.primitiveOffset = static_cast<std::uint32_t>(std::size(m_gIndices))
	};

	std::ranges::copy(primIndices, std::back_inserter(m_gIndices));
	std::ranges::copy(vertexIndices, std::back_inserter(m_gVerticesIndices));

	return meshlet;
}

std::uint32_t ModelProcessor::GetPrimIndex(
	std::uint32_t vIndex, std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesM,
	std::vector<std::uint32_t>& vertexIndices
) const noexcept {
	if (!IsInMap(vertexIndicesM, vIndex)) {
		vertexIndicesM.emplace(
			vIndex, static_cast<std::uint32_t>(std::size(vertexIndices))
		);
		vertexIndices.emplace_back(vIndex);
	}

	return vertexIndicesM[vIndex];
}

std::uint32_t ModelProcessor::GetExtraVertexCount(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesM,
	std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
) const noexcept {
	std::uint32_t extraVertexCount = 0u;

	if (!IsInMap(vertexIndicesM, primIndex1))
		++extraVertexCount;

	if (!IsInMap(vertexIndicesM, primIndex2))
		++extraVertexCount;

	if (!IsInMap(vertexIndicesM, primIndex3))
		++extraVertexCount;

	return extraVertexCount;
}

bool ModelProcessor::IsInMap(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesM,
	std::uint32_t vIndex
) const noexcept {
	return vertexIndicesM.find(vIndex) != std::end(vertexIndicesM);
}
*/
