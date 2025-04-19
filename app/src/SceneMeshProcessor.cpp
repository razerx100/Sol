#include <SceneMeshProcessor.hpp>
#include <MeshBoundImpl.hpp>
#include <MeshletMaker.hpp>
#include <concepts>
#include <type_traits>

MeshBundleTemporaryData SceneMeshProcessor::GenerateTemporaryMeshData(bool meshShader)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiScene const* scene = m_scene->GetScene();

	if (meshShader)
		meshBundleTempData = GenerateMeshShaderData(scene);
	else
		meshBundleTempData = GenerateVertexShaderData(scene);

	return meshBundleTempData;
}

void SceneMeshProcessor::SetSceneProcessor(std::shared_ptr<SceneProcessor> scene)
{
	m_scene = std::move(scene);
}

MeshBundleTemporaryData SceneMeshProcessor::GenerateMeshShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsMS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshMS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

MeshBundleTemporaryData SceneMeshProcessor::GenerateVertexShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsVS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshVS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

void SceneMeshProcessor::ProcessMeshVS(
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

void SceneMeshProcessor::ProcessMeshMS(
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

void SceneMeshProcessor::ProcessMeshVertices(
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

void SceneMeshProcessor::ProcessMeshFaces(
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

// Scene Mesh Processor
namespace SceneMeshProcessor1
{
	template<std::integral T>
	static void CopyIndices(
		std::vector<std::uint32_t>& indices,
		const tinygltf::Accessor& indicesAccessor, const tinygltf::BufferView& indexBufferView,
		const tinygltf::Buffer& indexBuffer, size_t oldOffset, size_t indexCount,
		std::uint32_t vertexOffset
	) {
		auto srcIndexData = reinterpret_cast<T const*>(
			std::data(indexBuffer.data) + indexBufferView.byteOffset + indicesAccessor.byteOffset
		);

		for (size_t index = 0u; index < indexCount; ++index)
			indices[oldOffset + index] = vertexOffset + srcIndexData[index];
	}

	template<typename T>
	static void CopyVertices(
		std::vector<Vertex>& vertices, size_t attributeIndex,
		const std::vector<tinygltf::Accessor>& accessors,
		const std::vector<tinygltf::BufferView>& bufferViews,
		const std::vector<tinygltf::Buffer>& buffers, size_t oldOffset,
		T Vertex::* vertexMember, bool flipZ
	) {
		if (attributeIndex != std::numeric_limits<size_t>::max())
		{
			const tinygltf::Accessor& accessor     = accessors[attributeIndex];

			const size_t vertexCount               = accessor.count;

			const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];

			const tinygltf::Buffer& buffer         = buffers[bufferView.buffer];

			auto srcData = reinterpret_cast<T const*>(
				std::data(buffer.data) + bufferView.byteOffset + accessor.byteOffset
			);

			const float zMultiplier = flipZ ? -1.f : 1.f;

			for (size_t index = 0u; index < vertexCount; ++index)
			{
				vertices[oldOffset + index].*vertexMember = srcData[index];

				if constexpr (std::is_same_v<T, DirectX::XMFLOAT3>)
					(vertices[oldOffset + index].*vertexMember).z *= zMultiplier;
			}
		}
	}

	[[nodiscard]]
	static std::uint32_t ProcessIndices(
		std::uint32_t vertexOffset, const tinygltf::Accessor& indicesAccessor,
		const std::vector<tinygltf::BufferView>& bufferViews,
		const std::vector<tinygltf::Buffer>& buffers, MeshBundleTemporaryData& meshBundleTempData
	) {
		const size_t indexCount = indicesAccessor.count;

		assert(indicesAccessor.type == TINYGLTF_TYPE_SCALAR && "Indices type isn't scalar.");

		std::vector<std::uint32_t>& indices = meshBundleTempData.indices;

		const size_t oldIndexCount          = std::size(indices);

		indices.resize(oldIndexCount + indexCount);

		const tinygltf::BufferView& indexBufferView = bufferViews[indicesAccessor.bufferView];

		const tinygltf::Buffer& indexBuffer         = buffers[indexBufferView.buffer];

		if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			CopyIndices<std::uint32_t>(
				indices, indicesAccessor, indexBufferView, indexBuffer, oldIndexCount, indexCount,
				vertexOffset
			);
		else if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			CopyIndices<std::uint16_t>(
				indices, indicesAccessor, indexBufferView, indexBuffer, oldIndexCount, indexCount,
				vertexOffset
			);
		else if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			CopyIndices<std::uint8_t>(
				indices, indicesAccessor, indexBufferView, indexBuffer, oldIndexCount, indexCount,
				vertexOffset
			);

		return static_cast<std::uint32_t>(indexCount);
	}

	static void ProcessVertices(
		const tinygltf::Primitive& primitive, const std::vector<tinygltf::Accessor>& accessors,
		const std::vector<tinygltf::BufferView>& bufferViews,
		const std::vector<tinygltf::Buffer>& buffers, MeshBundleTemporaryData& meshBundleTempData,
		AxisAlignedBoundingBox& aabb
	) {
		auto positionIndex = std::numeric_limits<size_t>::max();
		auto normalIndex   = std::numeric_limits<size_t>::max();
		auto uvIndex       = std::numeric_limits<size_t>::max();

		for (const auto& [name, accessorIndex] : primitive.attributes)
		{
			if (name == "POSITION")
				positionIndex = static_cast<size_t>(accessorIndex);
			else if (name == "NORMAL")
				normalIndex = static_cast<size_t>(accessorIndex);
			else if (name == "TEXCOORD_0")
				uvIndex = static_cast<size_t>(accessorIndex);
		}

		assert(positionIndex != std::numeric_limits<size_t>::max() && "There is no position.");

		const tinygltf::Accessor& positionAccessor = accessors[positionIndex];

		const size_t vertexCount      = positionAccessor.count;

		std::vector<Vertex>& vertices = meshBundleTempData.vertices;

		const size_t oldVertexCount   = std::size(vertices);

		Vertex vertex
		{
			.position = DirectX::XMFLOAT3{ 0.f, 0.f, 0.f },
			.normal   = DirectX::XMFLOAT3{ 0.f, 0.f, 0.f },
			.uv       = DirectX::XMFLOAT2{ 0.f, 0.f }
		};

		vertices.resize(oldVertexCount + vertexCount, vertex);

		// Flip Zs, as gltf is based on openGL which uses a Right Hand Coordinate system.

		{
			const tinygltf::BufferView& positionBufferView
				= bufferViews[positionAccessor.bufferView];

			const tinygltf::Buffer& positionBuffer = buffers[positionBufferView.buffer];

			auto srcPositionData = reinterpret_cast<DirectX::XMFLOAT3 const*>(
				std::data(positionBuffer.data) + positionBufferView.byteOffset
				+ positionAccessor.byteOffset
			);

			AABBGenerator aabbGen{};

			for (size_t index = 0u; index < vertexCount; ++index)
			{
				DirectX::XMFLOAT3& position = vertices[oldVertexCount + index].position;

				position    = srcPositionData[index];

				position.z *= -1.f;

				aabbGen.ProcessAxes(position);
			}

			aabb = aabbGen.GenerateAABB();
		}

		CopyVertices(
			vertices, normalIndex, accessors, bufferViews, buffers, oldVertexCount,
			&Vertex::normal, true
		);
		CopyVertices(
			vertices, uvIndex, accessors, bufferViews, buffers, oldVertexCount,
			&Vertex::uv, false
		);
	}

	static void ProcessMeshMS(
		const tinygltf::Mesh& mesh, const std::vector<tinygltf::Accessor>& accessors,
		const std::vector<tinygltf::BufferView>& bufferViews,
		const std::vector<tinygltf::Buffer>& buffers, MeshBundleTemporaryData& meshBundleTempData
	) {
		std::vector<Vertex>& vertices               = meshBundleTempData.vertices;
		std::vector<std::uint32_t>& indices         = meshBundleTempData.indices;
		std::vector<std::uint32_t>& primIndices     = meshBundleTempData.primIndices;
		std::vector<MeshletDetails>& meshletDetails = meshBundleTempData.meshletDetails;

		MeshTemporaryDetailsMS meshDetailsMS
		{
			.meshletOffset   = static_cast<std::uint32_t>(std::size(meshletDetails)),
			.indexOffset     = static_cast<std::uint32_t>(std::size(indices)),
			.primitiveOffset = static_cast<std::uint32_t>(std::size(primIndices)),
			.vertexOffset    = static_cast<std::uint32_t>(std::size(vertices))
		};

		AxisAlignedBoundingBox meshAabb{};

		for (const tinygltf::Primitive& primitive : mesh.primitives)
		{
			// For now only process Triangles.
			if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
				continue;

			// Process the vertices first, as we would want to generate Bounding Volumes
			// with them.
			ProcessVertices(
				primitive, accessors, bufferViews, buffers, meshBundleTempData, meshAabb
			);

			const tinygltf::Accessor& indicesAccessor = accessors[primitive.indices];

			{
				const size_t indexCount   = indicesAccessor.count;
				const size_t vertexOffset = meshDetailsMS.vertexOffset;

				assert(
					indicesAccessor.type == TINYGLTF_TYPE_SCALAR && "Indices type isn't scalar."
				);

				{
					const size_t currentMeshletDetailsCount = std::size(meshletDetails);
					const size_t potentialNewMeshletDetailsCount
						= indexCount / MeshletGenerator::s_meshletVertexLimit + 1u;

					meshletDetails.reserve(
						currentMeshletDetailsCount + potentialNewMeshletDetailsCount
					);
				}

				const tinygltf::BufferView& indexBufferView
					= bufferViews[indicesAccessor.bufferView];

				const tinygltf::Buffer& indexBuffer = buffers[indexBufferView.buffer];

				if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					auto srcIndexData = reinterpret_cast<std::uint32_t const*>(
						std::data(indexBuffer.data) + indexBufferView.byteOffset
						+ indicesAccessor.byteOffset
					);

					MeshletGenerator meshletGen{ indices, primIndices };

					AABBGenerator spatialAabbGen{};
					AABBGenerator normalAabbGen{};

					for (size_t index = 0u; index + 2u < indexCount; index += 3u)
					{
						MeshletGenerator::PrimTriangle triangle
						{
							.vertex0 = srcIndexData[index + 0u],
							.vertex1 = srcIndexData[index + 1u],
							.vertex2 = srcIndexData[index + 2u]
						};

						const Vertex& vertex0 = vertices[vertexOffset + triangle.vertex0];
						const Vertex& vertex1 = vertices[vertexOffset + triangle.vertex1];
						const Vertex& vertex2 = vertices[vertexOffset + triangle.vertex2];

						spatialAabbGen.ProcessAxes(vertex0.position);
						spatialAabbGen.ProcessAxes(vertex1.position);
						spatialAabbGen.ProcessAxes(vertex2.position);

						normalAabbGen.ProcessAxes(vertex0.normal);
						normalAabbGen.ProcessAxes(vertex1.normal);
						normalAabbGen.ProcessAxes(vertex2.normal);

						const bool isMeshletProcessed = meshletGen.ProcessPrimitive(triangle);

						if (isMeshletProcessed)
						{
							SphereBVGenerator spatialSphereBVGen{};

							spatialSphereBVGen.SetCentre(spatialAabbGen.GenerateAABB());

							NormalConeGenerator normalConeGen{};

							// We don't need to wait for the radius computation. We only need
							// the centre.
							normalConeGen.SetSpatialCentre(spatialSphereBVGen.GenerateBV());

							{
								SphereBVGenerator normalSphereBVGen{};

								normalSphereBVGen.SetCentre(normalAabbGen.GenerateAABB());

								normalConeGen.SetNormalCentre(normalSphereBVGen.GenerateBV());
							}

							// Since the indices containers are per mesh bundle here, we must
							// set the correct mesh offsets.
							const Meshlet meshlet = meshletGen.GenerateMeshlet(
								meshDetailsMS.indexOffset, meshDetailsMS.primitiveOffset
							);

							const size_t meshIndexOffset    = meshDetailsMS.indexOffset;
							const size_t meshletIndexOffset = meshlet.indexOffset;
							const size_t indexOffset        = meshIndexOffset + meshletIndexOffset;

							const size_t meshletIndexCount  = meshlet.indexCount;

							// Going through all the vertices of the meshlet.
							for (size_t index1 = indexOffset; index1 < meshletIndexCount; ++index1)
							{
								const Vertex& vertex = vertices[vertexOffset + indices[index1]];

								spatialSphereBVGen.ProcessRadius(vertex.position);

								normalConeGen.ProcessNormalMinimumDot(vertex.normal);
								normalConeGen.ProcessApexOffset(vertex.position, vertex.normal);
							}

							meshletDetails.emplace_back(
								MeshletDetails
								{
									.meshlet    = meshlet,
									.sphereB    = spatialSphereBVGen.GenerateBV(),
									.coneNormal = normalConeGen.GenerateNormalCone()
								}
							);

							spatialAabbGen = AABBGenerator{};
							normalAabbGen  = AABBGenerator{};
							// Need to do this to reset the offsets and the vertex index map.
							meshletGen     = MeshletGenerator{ indices, primIndices };
						}
					}
				}
			}

			// Hopefully, there can't be multiple instances of the same mode?
		}

		meshDetailsMS.aabb = meshAabb;
		meshDetailsMS.meshletCount
			= static_cast<std::uint32_t>(std::size(meshletDetails) - meshDetailsMS.meshletOffset);

		meshBundleTempData.bundleDetails.meshTemporaryDetailsMS.emplace_back(meshDetailsMS);
	}

	static void GenerateMeshShaderData(
		const tinygltf::Model& gltf, MeshBundleTemporaryData& meshBundleTempData
	) {
		const size_t meshCount = std::size(gltf.meshes);

		meshBundleTempData.bundleDetails.meshTemporaryDetailsMS.reserve(meshCount);

		for (size_t index = 0u; index < meshCount; ++index)
			ProcessMeshMS(
				gltf.meshes[index], gltf.accessors, gltf.bufferViews, gltf.buffers,
				meshBundleTempData
			);
	}

	static void ProcessMeshVS(
		const tinygltf::Mesh& mesh, const std::vector<tinygltf::Accessor>& accessors,
		const std::vector<tinygltf::BufferView>& bufferViews,
		const std::vector<tinygltf::Buffer>& buffers, MeshBundleTemporaryData& meshBundleTempData
	) {
		MeshTemporaryDetailsVS meshDetailsVS
		{
			// Should be all triangles.
			.indexOffset = static_cast<std::uint32_t>(std::size(meshBundleTempData.indices)),
		};

		AxisAlignedBoundingBox aabb{};

		for (const tinygltf::Primitive& primitive : mesh.primitives)
		{
			// For now only process Triangles.
			if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
				continue;

			const tinygltf::Accessor& indicesAccessor = accessors[primitive.indices];

			const auto vertexOffset = static_cast<std::uint32_t>(
				std::size(meshBundleTempData.vertices)
			);

			meshDetailsVS.indexCount = ProcessIndices(
				vertexOffset, indicesAccessor, bufferViews, buffers, meshBundleTempData
			);

			ProcessVertices(
				primitive, accessors, bufferViews, buffers, meshBundleTempData, aabb
			);
			// Hopefully, there can't be multiple instances of the same mode?
		}

		meshDetailsVS.aabb = aabb;

		meshBundleTempData.bundleDetails.meshTemporaryDetailsVS.emplace_back(meshDetailsVS);
	}

	static void GenerateVertexShaderData(
		const tinygltf::Model& gltf, MeshBundleTemporaryData& meshBundleTempData
	) {
		const size_t meshCount = std::size(gltf.meshes);

		meshBundleTempData.bundleDetails.meshTemporaryDetailsVS.reserve(meshCount);

		for (size_t index = 0u; index < meshCount; ++index)
			ProcessMeshVS(
				gltf.meshes[index], gltf.accessors, gltf.bufferViews, gltf.buffers,
				meshBundleTempData
			);
	}

	MeshBundleTemporaryData GenerateTemporaryMeshData(GLTFObject& gltfObj, bool meshShader)
	{
		MeshBundleTemporaryData meshBundleTempData{};

		const tinygltf::Model& gltf = gltfObj.Get();

		if (meshShader)
			GenerateMeshShaderData(gltf, meshBundleTempData);
		else
			GenerateVertexShaderData(gltf, meshBundleTempData);

		return meshBundleTempData;
	}
}
