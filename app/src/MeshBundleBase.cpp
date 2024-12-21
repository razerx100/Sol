#include <MeshBundleBase.hpp>
#include <ranges>
#include <algorithm>
#include <cassert>
#include <MeshBoundImpl.hpp>
#include <BoundingVolumes.hpp>
#include <Exception.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <ConversionUtilities.hpp>

template<typename T>
static void MemcpyIntoVector(std::vector<T>& dst, const std::vector<T>& src) noexcept
{
	const size_t srcElementCount = std::size(src);
	const size_t dstElementCount = std::size(dst);

	const size_t srcDataSize  = sizeof(T) * srcElementCount;
	// Since std::data returns T*, the start would be the old count.
	const size_t dstDataStart = dstElementCount;

	dst.resize(srcElementCount + dstElementCount);

	memcpy(std::data(dst) + dstDataStart, std::data(src), srcDataSize);
}

static void CopyAndOffsetIndices(
	std::vector<std::uint32_t>& dst, const std::vector<std::uint32_t>& src, std::uint32_t offset
) noexcept {
	const size_t srcElementCount = std::size(src);
	const size_t dstElementCount = std::size(dst);

	const size_t dstDataStart    = dstElementCount;

	dst.resize(srcElementCount + dstElementCount);

	for (size_t index = 0u; index < srcElementCount; ++index)
		dst[index + dstDataStart] = offset + src[index];
}

// Mesh Bundle Temp Custom
MeshBundleTemporaryData MeshBundleTempCustom::GenerateTemporaryData(bool meshShader)
{
	MeshBundleTemporaryData meshBundleTempData{};

	if (meshShader)
		GenerateMeshShaderData(m_tempMeshes, meshBundleTempData);
	else
		GenerateVertexShaderData(m_tempMeshes, meshBundleTempData);

	m_tempMeshes = std::vector<Mesh>{};

	return meshBundleTempData;
}

void MeshBundleTempCustom::GenerateMeshShaderData(
	std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
) {
	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.reserve(std::size(meshes));

	for (Mesh& mesh : meshes)
		ProcessMeshMS(mesh, meshBundleTemporaryData);
}

void MeshBundleTempCustom::GenerateVertexShaderData(
	std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
) {
	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.reserve(std::size(meshes));

	for (Mesh& mesh : meshes)
		ProcessMeshVS(mesh, meshBundleTemporaryData);
}

void MeshBundleTempCustom::ProcessMeshVS(
	Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshTemporaryDetailsVS meshDetailsVS
	{
		.indexCount   = static_cast<std::uint32_t>(std::size(mesh.indices)),
		.indexOffset  = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.aabb         = GenerateAABB(mesh.vertices)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.emplace_back(meshDetailsVS);

	const auto vertexOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices));

	CopyAndOffsetIndices(meshBundleTemporaryData.indices, mesh.indices, vertexOffset);
	MemcpyIntoVector(meshBundleTemporaryData.vertices, mesh.vertices);
}

void MeshBundleTempCustom::ProcessMeshMS(
	Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshletMaker meshletMaker{};
	meshletMaker.GenerateMeshlets(mesh);

	meshletMaker.LoadVertexIndices(mesh.indices);

	MeshExtraForMesh extraMeshData = meshletMaker.GenerateExtraMeshData();

	MeshTemporaryDetailsMS meshDetailsMS
	{
		.meshletCount    = static_cast<std::uint32_t>(std::size(extraMeshData.meshletDetails)),
		.meshletOffset   = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.meshletDetails)),
		.primitiveOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.primIndices)),
		.vertexOffset    = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices)),
		.aabb            = GenerateAABB(mesh.vertices)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.emplace_back(meshDetailsMS);

	{
		// Per meshlet Bounding Sphere
		std::vector<MeshletDetails>& meshletDetails = extraMeshData.meshletDetails;

		const size_t meshletCount = std::size(meshletDetails);

		for (size_t index = 0u; index < meshletCount; ++index)
		{
			MeshletDetails& meshletDetail = meshletDetails[index];

			meshletDetail.sphereB = GenerateSphereBV(
				mesh.vertices, mesh.indices, meshletDetail.meshlet
			);

			meshletDetail.coneNormal = GenerateNormalCone(
				mesh.vertices, mesh.indices, extraMeshData.primIndices, meshletDetail
			);
		}
	}

	MemcpyIntoVector(meshBundleTemporaryData.meshletDetails, extraMeshData.meshletDetails);
	MemcpyIntoVector(meshBundleTemporaryData.primIndices, extraMeshData.primIndices);
	MemcpyIntoVector(meshBundleTemporaryData.indices, mesh.indices);
	MemcpyIntoVector(meshBundleTemporaryData.vertices, mesh.vertices);
}

void MeshBundleTempCustom::AddMesh(Mesh&& mesh) noexcept
{
	m_tempMeshes.emplace_back(std::move(mesh));
}

// Mesh Bundle Temp Assimp
MeshBundleTempAssimp::~MeshBundleTempAssimp() noexcept
{
	if (m_tempScene)
		aiReleaseImport(m_tempScene);
}

void MeshBundleTempAssimp::ProcessMeshVertices(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	size_t newVertexCount = mesh->mNumVertices;
	aiVector3D* vertices  = mesh->mVertices;
	// Only using the first coordinates.
	aiVector3D* uvs       = mesh->mTextureCoords[0];
	aiVector3D* normals   = mesh->mNormals;

	std::vector<Vertex>& bundleVertices = meshBundleTemporaryData.vertices;

	bundleVertices.reserve(std::size(bundleVertices) + newVertexCount);

	if (mesh->HasTextureCoords(0))
	{
		for (size_t index = 0u; index < newVertexCount; ++index)
		{
			Vertex vertex
			{
				.position = GetXMFloat3(vertices[index]),
				.normal   = GetXMFloat3(normals[index]),
				.uv       = DirectX::XMFLOAT2{ uvs[index].x, uvs[index].y }
			};

			bundleVertices.emplace_back(vertex);
		}
	}
	else
	{
		for (size_t index = 0u; index < newVertexCount; ++index)
		{
			Vertex vertex
			{
				.position = GetXMFloat3(vertices[index]),
				.normal   = GetXMFloat3(normals[index]),
				.uv       = DirectX::XMFLOAT2{ 0.f, 0.f }
			};

			bundleVertices.emplace_back(vertex);
		}
	}
}

void MeshBundleTempAssimp::ProcessMeshFaces(
	aiMesh* mesh, std::uint32_t vertexOffset, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	size_t faceCount           = mesh->mNumFaces;
	const size_t newIndexCount = faceCount * 3u;
	aiFace* faces              = mesh->mFaces;

	std::vector<std::uint32_t>& bundleIndices = meshBundleTemporaryData.indices;

	bundleIndices.reserve(std::size(bundleIndices) + newIndexCount);

	for (size_t index = 0u; index < faceCount; ++index)
	{
		const aiFace& face = faces[index];

		// Should be all triangles.
		std::uint32_t vIndex0 = vertexOffset + face.mIndices[0];
		std::uint32_t vIndex1 = vertexOffset + face.mIndices[1];
		std::uint32_t vIndex2 = vertexOffset + face.mIndices[2];

		bundleIndices.emplace_back(vIndex0);
		bundleIndices.emplace_back(vIndex1);
		bundleIndices.emplace_back(vIndex2);
	}
}

void MeshBundleTempAssimp::ProcessMeshVS(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshTemporaryDetailsVS meshDetailsVS
	{
		// Should be all triangles.
		.indexCount   = mesh->mNumFaces * 3u,
		.indexOffset  = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.aabb         = GetAABB(mesh->mAABB)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.emplace_back(meshDetailsVS);

	const auto vertexOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices));

	ProcessMeshVertices(mesh, meshBundleTemporaryData);

	ProcessMeshFaces(mesh, vertexOffset, meshBundleTemporaryData);
}

void MeshBundleTempAssimp::ProcessMeshMS(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshletMaker meshletMaker{};

	meshletMaker.GenerateMeshlets(mesh);

	std::vector<std::uint32_t> vertexIndices{};

	meshletMaker.LoadVertexIndices(vertexIndices);

	MeshExtraForMesh extraMeshData = meshletMaker.GenerateExtraMeshData();

	MeshTemporaryDetailsMS meshDetailsMS
	{
		.meshletCount    = static_cast<std::uint32_t>(std::size(extraMeshData.meshletDetails)),
		.meshletOffset   = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.meshletDetails)),
		.indexOffset     = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.primitiveOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.primIndices)),
		.vertexOffset    = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices)),
		.aabb            = GetAABB(mesh->mAABB)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.emplace_back(meshDetailsMS);

	{
		// Per meshlet Bounding Sphere
		std::vector<MeshletDetails>& meshletDetails = extraMeshData.meshletDetails;

		const size_t meshletCount = std::size(meshletDetails);

		for (size_t index1 = 0u; index1 < meshletCount; ++index1)
		{
			MeshletDetails& meshletDetail = meshletDetails[index1];

			meshletDetail.sphereB = GenerateSphereBV(
				mesh->mVertices, vertexIndices, meshletDetail.meshlet
			);

			meshletDetail.coneNormal = GenerateNormalCone(
				mesh->mVertices, mesh->mNormals, vertexIndices, extraMeshData.primIndices,
				meshletDetail
			);
		}
	}

	MemcpyIntoVector(meshBundleTemporaryData.meshletDetails, extraMeshData.meshletDetails);
	MemcpyIntoVector(meshBundleTemporaryData.primIndices, extraMeshData.primIndices);
	MemcpyIntoVector(meshBundleTemporaryData.indices, vertexIndices);

	ProcessMeshVertices(mesh, meshBundleTemporaryData);
}

MeshBundleTemporaryData MeshBundleTempAssimp::GenerateMeshShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsMS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshMS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

MeshBundleTemporaryData MeshBundleTempAssimp::GenerateVertexShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsVS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshVS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

MeshBundleTemporaryData MeshBundleTempAssimp::GenerateTemporaryData(bool meshShader)
{
	MeshBundleTemporaryData meshBundleTempData{};

	if (meshShader)
		meshBundleTempData = GenerateMeshShaderData(m_tempScene);
	else
		meshBundleTempData = GenerateVertexShaderData(m_tempScene);

	aiReleaseImport(m_tempScene);

	m_tempScene = nullptr;

	return meshBundleTempData;
}

void MeshBundleTempAssimp::SetMeshBundle(const std::string& fileName)
{
	m_tempScene = aiImportFile(
		fileName.c_str(),
		static_cast<std::uint32_t>(
		aiProcess_CalcTangentSpace      |
		aiProcess_Triangulate           |
		aiProcess_JoinIdenticalVertices |
		aiProcess_MakeLeftHanded        |
		aiProcess_FlipUVs               |
		aiProcess_FlipWindingOrder      |
		aiProcess_GenBoundingBoxes      |
		aiProcess_GenNormals            |
		aiProcess_GenUVCoords           |
		aiProcess_SortByPType
		)
	);

	if (!m_tempScene)
		throw Exception{ "MeshFileMissing", "The mesh file couldn't be found." };
}

void MeshBundleTempAssimp::ProcessMeshNodeDetails(
	aiNode const* node, std::vector<MeshNodeData>& meshNodeData, aiMesh** meshes,
	std::uint32_t& childrenOffset, std::uint32_t& modelIndex
) {
	const std::uint32_t childCount = node->mNumChildren;

	auto currentModelIndex = std::numeric_limits<std::uint32_t>::max();
	auto meshIndex         = std::numeric_limits<std::uint32_t>::max();

	if (node->mNumMeshes)
	{
		const size_t meshCount = node->mNumMeshes;

		for (size_t index = 0u; index < meshCount; ++index)
		{
			size_t currentMeshIndex = node->mMeshes[index];
			aiMesh* mesh            = meshes[currentMeshIndex];

			const bool isTriangle = mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE;

			// Skipping non triangles.
			if (!isTriangle)
				continue;

			// Multiple meshes per node isn't supported yet.
			meshIndex = static_cast<std::uint32_t>(currentMeshIndex);
			break;
		}

		currentModelIndex = modelIndex;
		++modelIndex;
	}

	meshNodeData.emplace_back(
		MeshNodeData
		{
			.modelIndex   = currentModelIndex,
			.meshIndex    = meshIndex,
			.childrenData = MeshChildrenData{ .count = childCount, .startingIndex = childrenOffset }
		}
	);

	childrenOffset += childCount;
}

void MeshBundleTempAssimp::TraverseMeshHierarchyDetails(
	aiNode const* node,
	DirectX::XMMATRIX accumulatedTransform, std::vector<MeshPermanentDetails>& permanentDetails,
	std::vector<MeshNodeData>& meshNodeData, aiMesh** meshes, std::uint32_t& childrenOffset,
	std::uint32_t& modelIndex
) {
	using namespace DirectX;

	size_t childCount = node->mNumChildren;
	aiNode** children = node->mChildren;

	for (size_t index = 0u; index < childCount; ++index)
	{
		aiNode const* child = children[index];

		if (child->mNumMeshes)
		{
			// I pass row major matrices in the shaders, and assimp loads column major matrices.
			XMMATRIX tempAccumulatedTransform =
				accumulatedTransform * XMMatrixTranspose(GetXMMatrix(child->mTransformation));

			permanentDetails.emplace_back(
				MeshPermanentDetails{ .worldMatrix = tempAccumulatedTransform }
			);
		}


		ProcessMeshNodeDetails(child, meshNodeData, meshes, childrenOffset, modelIndex);
	}

	for (size_t index = 0u; index < childCount; ++index)
	{
		aiNode const* child = children[index];

		XMMATRIX tempAccumulatedTransform = accumulatedTransform;

		if (child->mNumMeshes)
			tempAccumulatedTransform =
				tempAccumulatedTransform * XMMatrixTranspose(GetXMMatrix(child->mTransformation));

		TraverseMeshHierarchyDetails(
			child, tempAccumulatedTransform, permanentDetails, meshNodeData, meshes, childrenOffset,
			modelIndex
		);
	}
}

void MeshBundleTempAssimp::FillMeshHierarchyDetails(
	std::vector<MeshPermanentDetails>& permanentDetails, std::vector<MeshNodeData>& meshNodeData
) {
	using namespace DirectX;

	aiNode const* rootNode        = m_tempScene->mRootNode;
	XMMATRIX accumulatedTransform = XMMatrixIdentity();
	std::uint32_t childrenOffset  = 1u;

	const size_t meshCount   = rootNode->mNumMeshes;

	std::uint32_t modelIndex = 0u;

	// There can be more nodes than the mesh count, but in most cases it won't be.
	permanentDetails.reserve(meshCount);
	meshNodeData.reserve(meshCount);

	ProcessMeshNodeDetails(rootNode, meshNodeData, m_tempScene->mMeshes, childrenOffset, modelIndex);

	if (rootNode->mNumMeshes)
	{
		// Calculate the transform for the root.
		// I pass row major matrices in the shaders, and assimp loads column major matrices.
		accumulatedTransform =
			accumulatedTransform * XMMatrixTranspose(GetXMMatrix(rootNode->mTransformation));

		permanentDetails.emplace_back(MeshPermanentDetails{ .worldMatrix = accumulatedTransform });
	}

	TraverseMeshHierarchyDetails(
		rootNode, accumulatedTransform, permanentDetails, meshNodeData, m_tempScene->mMeshes,
		childrenOffset, modelIndex
	);
}

// Mesh Bundle Temporary Impl
MeshBundleTemporaryImpl::MeshBundleTemporaryImpl(bool customTemp)
	: m_tempData{},
	m_tempIntermediate{ customTemp ? std::make_unique<MeshBundleTempCustom>() : nullptr }
{}

void MeshBundleTemporaryImpl::GenerateTemporaryData(bool meshShader)
{
	m_tempData = m_tempIntermediate->GenerateTemporaryData(meshShader);
}

void MeshBundleTemporaryImpl::AddMesh(Mesh&& mesh)
{
	assert(dynamic_cast<MeshBundleTempCustom*>(m_tempIntermediate.get()) && "Temp type isn't custom.");

	m_tempIntermediate->AddMesh(std::move(mesh));
}

void MeshBundleTemporaryImpl::SetMeshBundle(
	const std::string& fileName,
	std::vector<MeshPermanentDetails>& permanentDetails, std::vector<MeshNodeData>& meshNodeData
) {
	if (!m_tempIntermediate)
	{
		auto tempData = std::make_unique<MeshBundleTempAssimp>();

		tempData->SetMeshBundle(fileName);
		tempData->FillMeshHierarchyDetails(permanentDetails, meshNodeData);

		m_tempIntermediate = std::move(tempData);
	}
}

// Mesh Bundle Impl
void MeshBundleImpl::AddMesh(Mesh&& mesh)
{
	m_temporaryData->AddMesh(std::move(mesh));

	m_meshNodeData.emplace_back(
		MeshNodeData
		{
			.meshIndex    = 0u,
			.childrenData = MeshChildrenData{ .count = 0u, .startingIndex = 0u }
		}
	);
	m_permanentDetails.emplace_back(MeshPermanentDetails{ DirectX::XMMatrixIdentity() });
}

void MeshBundleImpl::SetMeshBundle(const std::string& fileName)
{
	m_temporaryData->SetMeshBundle(fileName, m_permanentDetails, m_meshNodeData);
}

void MeshBundleImpl::CalculateNormalsIndependentFaces(
	std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
) noexcept {
	for (size_t index = 0u; index < std::size(indices); index += 3)
	{
		Vertex& vertex1 = vertices[indices[index]];
		Vertex& vertex2 = vertices[indices[index + 1u]];
		Vertex& vertex3 = vertices[indices[index + 2u]];

		DirectX::XMFLOAT3 faceNormal = GetFaceNormal(
			vertex1.position, vertex2.position, vertex3.position
		);

		vertex1.normal = faceNormal;
		vertex2.normal = faceNormal;
		vertex3.normal = faceNormal;
	}
}

DirectX::XMFLOAT3 MeshBundleImpl::GetFaceNormal(
	const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
	const DirectX::XMFLOAT3& position3
) noexcept {
	DirectX::XMVECTOR vertex1 = DirectX::XMLoadFloat3(&position1);
	DirectX::XMVECTOR vertex2 = DirectX::XMLoadFloat3(&position2);
	DirectX::XMVECTOR vertex3 = DirectX::XMLoadFloat3(&position3);

	// Edges should be calculated in this subtraction order for ClockWise triangle vertices
	// drawing order
	DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(vertex2, vertex1);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(vertex3, vertex1);

	DirectX::XMVECTOR faceNormalV = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(edge1, edge2)
	);
	DirectX::XMFLOAT3 faceNormal{};
	DirectX::XMStoreFloat3(&faceNormal, faceNormalV);

	return faceNormal;
}

void MeshBundleImpl::SetUVToVertices(
	std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
) noexcept {
	for (size_t index = 0u; index < std::size(uvs); ++index)
		vertices[index].uv = uvs[index];
}

// Meshlet Maker
MeshletMaker::MeshletMaker()
	: m_tempVertexIndices{}, m_tempPrimitiveIndices{}, m_vertexIndices{}, m_primitiveIndices{},
	m_meshletDetails{}
{
	m_tempVertexIndices.reserve(s_meshletVertexLimit);
	m_tempPrimitiveIndices.reserve(s_meshletPrimitiveLimit);
}

void MeshletMaker::GenerateMeshlets(const Mesh& mesh) noexcept
{
	const std::vector<std::uint32_t>& indices = mesh.indices;

	for (size_t start = 0u; start < std::size(indices);)
		start = MakeMeshlet(indices, start);
}

void MeshletMaker::GenerateMeshlets(aiMesh const* mesh) noexcept
{
	size_t faceCount = mesh->mNumFaces;
	aiFace* faces    = mesh->mFaces;

	for (size_t start = 0u; start < faceCount;)
		start = MakeMeshlet(faces, faceCount, start);
}

void MeshletMaker::LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices) noexcept
{
	vertexIndices = std::move(m_vertexIndices);
}

MeshExtraForMesh MeshletMaker::GenerateExtraMeshData() noexcept
{
	return MeshExtraForMesh
	{
		.primIndices    = std::move(m_primitiveIndices),
		.meshletDetails = std::move(m_meshletDetails)
	};
}

bool MeshletMaker::IsInMap(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::uint32_t vIndex
) noexcept {
	return vertexIndicesMap.find(vIndex) != std::end(vertexIndicesMap);
}

std::uint32_t MeshletMaker::GetPrimIndex(
	std::uint32_t vIndex, std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::vector<std::uint32_t>& vertexIndices
) noexcept {
	if (!IsInMap(vertexIndicesMap, vIndex))
	{
		vertexIndicesMap.emplace(
			vIndex, static_cast<std::uint32_t>(std::size(vertexIndices))
		);
		vertexIndices.emplace_back(vIndex);
	}

	return vertexIndicesMap[vIndex];
}

std::uint32_t MeshletMaker::GetExtraIndexCount(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
) noexcept {
	std::uint32_t extraIndexCount = 0u;

	if (!IsInMap(vertexIndicesMap, primIndex1))
		++extraIndexCount;

	if (!IsInMap(vertexIndicesMap, primIndex2))
		++extraIndexCount;

	if (!IsInMap(vertexIndicesMap, primIndex3))
		++extraIndexCount;

	return extraIndexCount;
}

size_t MeshletMaker::MakeMeshlet(const std::vector<std::uint32_t>& indices, size_t startingIndex) noexcept
{
	const size_t meshletIndexLimit = startingIndex + s_meshletPrimitiveLimit * 3u;

	// I would love to make this map into static or something but that isn't going to work. As least
	// in this approach. A meshlet should have 64 vertices and 126 primitives. The problem arises when
	// there is a huge mesh with million vertices, what if a triangle is formed by for example if the
	// 1st, 1M th, and the 999th vertices decide to make a triangle, it would be really hard to define
	// the vertex offset with the limit 64, if there is only a single instance of those vertex indices
	// So, making a new duplicate and adding it at the end would solve the issue. So, can't keep the map
	// from any previous meshlets. And since the unordered map is a linked_list, there is no point in
	// preserving it like a vector, as the allocation can't be reused. Maybe I can use the flatmap when it
	// is implemented on MSVC.
	std::unordered_map<std::uint32_t, std::uint32_t> vertexIndicesMap{};

	m_tempVertexIndices.clear();
	m_tempPrimitiveIndices.clear();

	size_t indexOffset                   = startingIndex;
	constexpr size_t triangleVertexCount = 3u;

	for (; indexOffset + 2u < std::size(indices) && indexOffset < meshletIndexLimit;
		indexOffset += triangleVertexCount)
	{
		const std::uint32_t vIndex1 = indices[indexOffset];
		const std::uint32_t vIndex2 = indices[indexOffset + 1u];
		const std::uint32_t vIndex3 = indices[indexOffset + 2u];

		const auto vIndexCount = static_cast<std::uint32_t>(std::size(m_tempVertexIndices));

		const std::uint32_t couldBeVIndexCount = vIndexCount + GetExtraIndexCount(
			vertexIndicesMap, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVIndexCount > s_meshletVertexLimit)
			break;

		// The prim indices are the local vertex indices. As in, they are local to the meshlet.
		// We are assuming each primitive would be a triangle. Each prim index actually has the
		// three local vertex indices which create that triangle. So, actually the primIndices
		// contain the indices of the primitive/triangles for that meshlet.

		// And the vertexIndices are the unique vertex indices. With the duplicates
		// removed. The indices of the primitives index into the vertexIndices and then that index
		// is used to find the actual vertex.
		const std::uint32_t primIndexOne   = GetPrimIndex(vIndex1, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexTwo   = GetPrimIndex(vIndex2, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(vIndex3, vertexIndicesMap, m_tempVertexIndices);

		PrimitiveIndicesUnpacked unpackedPrim
		{
			.firstIndex  = primIndexOne,
			.secondIndex = primIndexTwo,
			.thirdIndex  = primIndexThree
		};

		std::uint32_t packedPrim = 0u;

		memcpy(&packedPrim, &unpackedPrim, sizeof(std::uint32_t));

		m_tempPrimitiveIndices.emplace_back(packedPrim);
	}

	const auto vertexOffset    = static_cast<std::uint32_t>(std::size(m_vertexIndices));
	const auto primitiveOffset = static_cast<std::uint32_t>(std::size(m_primitiveIndices));

	m_meshletDetails.emplace_back(
		MeshletDetails
		{
			.meshlet = Meshlet
			{
				.indexCount      = static_cast<std::uint32_t>(std::size(m_tempVertexIndices)),
				.indexOffset     = static_cast<std::uint32_t>(vertexOffset),
				.primitiveCount  = static_cast<std::uint32_t>(std::size(m_tempPrimitiveIndices)),
				.primitiveOffset = static_cast<std::uint32_t>(primitiveOffset)
			}
		}
	);

	MemcpyIntoVector(m_vertexIndices, m_tempVertexIndices);
	MemcpyIntoVector(m_primitiveIndices, m_tempPrimitiveIndices);

	return indexOffset;
}

size_t MeshletMaker::MakeMeshlet(aiFace* faces, size_t faceCount, size_t startingIndex) noexcept
{
	const size_t meshletIndexLimit = startingIndex + s_meshletPrimitiveLimit;

	// I would love to make this map into static or something but that isn't going to work. As least
	// in this approach. A meshlet should have 64 vertices and 126 primitives. The problem arises when
	// there is a huge mesh with million vertices, what if a triangle is formed by for example if the
	// 1st, 1M th, and the 999th vertices decide to make a triangle, it would be really hard to define
	// the vertex offset with the limit 64, if there is only a single instance of those vertex indices
	// So, making a new duplicate and adding it at the end would solve the issue. So, can't keep the map
	// from any previous meshlets. And since the unordered map is a linked_list, there is no point in
	// preserving it like a vector, as the allocation can't be reused. Maybe I can use the flatmap when it
	// is implemented on MSVC.
	std::unordered_map<std::uint32_t, std::uint32_t> vertexIndicesMap{};

	m_tempVertexIndices.clear();
	m_tempPrimitiveIndices.clear();

	size_t indexOffset = startingIndex;

	for (;indexOffset < faceCount && indexOffset < meshletIndexLimit; ++indexOffset)
	{
		const aiFace& face = faces[indexOffset];

		// Each face should be a triangle.
		const std::uint32_t vIndex1 = face.mIndices[0];
		const std::uint32_t vIndex2 = face.mIndices[1];
		const std::uint32_t vIndex3 = face.mIndices[2];

		const auto vIndexCount = static_cast<std::uint32_t>(std::size(m_tempVertexIndices));

		const std::uint32_t couldBeVIndexCount = vIndexCount + GetExtraIndexCount(
			vertexIndicesMap, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVIndexCount > s_meshletVertexLimit)
			break;

		// The prim indices are the local vertex indices. As in, they are local to the meshlet.
		// We are assuming each primitive would be a triangle. Each prim index actually has the
		// three local vertex indices which create that triangle. So, actually the primIndices
		// contain the indices of the primitive/triangles for that meshlet.

		// And the vertexIndices are the unique vertex indices. With the duplicates
		// removed. The indices of the primitives index into the vertexIndices and then that index
		// is used to find the actual vertex.
		const std::uint32_t primIndexOne   = GetPrimIndex(vIndex1, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexTwo   = GetPrimIndex(vIndex2, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(vIndex3, vertexIndicesMap, m_tempVertexIndices);

		PrimitiveIndicesUnpacked unpackedPrim
		{
			.firstIndex  = primIndexOne,
			.secondIndex = primIndexTwo,
			.thirdIndex  = primIndexThree
		};

		std::uint32_t packedPrim = 0u;

		memcpy(&packedPrim, &unpackedPrim, sizeof(std::uint32_t));

		m_tempPrimitiveIndices.emplace_back(packedPrim);
	}

	const auto localVertexOffset = static_cast<std::uint32_t>(std::size(m_vertexIndices));
	const auto primitiveOffset   = static_cast<std::uint32_t>(std::size(m_primitiveIndices));

	m_meshletDetails.emplace_back(
		MeshletDetails
		{
			.meshlet = Meshlet
			{
				.indexCount      = static_cast<std::uint32_t>(std::size(m_tempVertexIndices)),
				.indexOffset     = static_cast<std::uint32_t>(localVertexOffset),
				.primitiveCount  = static_cast<std::uint32_t>(std::size(m_tempPrimitiveIndices)),
				.primitiveOffset = static_cast<std::uint32_t>(primitiveOffset)
			}
		}
	);

	MemcpyIntoVector(m_vertexIndices, m_tempVertexIndices);
	MemcpyIntoVector(m_primitiveIndices, m_tempPrimitiveIndices);

	return indexOffset;
}

MeshletMaker::PrimitiveIndicesUnpacked MeshletMaker::UnpackPrim(std::uint32_t packedIndices) noexcept
{
	PrimitiveIndicesUnpacked unpackedIndices{ 0u, 0u, 0u };

	memcpy(&unpackedIndices, &packedIndices, sizeof(std::uint32_t));

	return unpackedIndices;
}
