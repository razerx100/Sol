#ifndef MODEL_PROCESSOR_HPP_
#define MODEL_PROCESSOR_HPP_
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <SolConcepts.hpp>
#include <optional>

#include <Model.hpp>

class ModelProcessor {
	struct IndexData {
		std::uint32_t indexCount;
		std::uint32_t indexOffset;
		ModelBoundingBox boundingBox;
	};

	union PrimitiveIndices {
		struct {
			std::uint32_t firstIndex : 10u;
			std::uint32_t secondIndex : 10u;
			std::uint32_t thirdIndex : 10u;
		}unpacked;
		std::uint32_t packed;
	};

public:
	struct Args {
		std::optional<bool> processingModeMeshlet;
	};

public:
	ModelProcessor(const Args& arguments) noexcept;

	template<DerivedWithoutArgs<ModelInputs> T>
	void AddModel(
		std::shared_ptr<Model> model, std::wstring pixelShader
	) noexcept {
		std::string inputName = ModelInputs::GetName<T>();

		if (!FindModelInput(inputName))
			AddModelInput(std::make_unique<T>(), inputName);

		_addModel(std::move(model), inputName, pixelShader);
	}

	template<DerivedWithArgs<ModelInputs> T>
	void AddModel(
		std::shared_ptr<Model> model, const typename T::Args& arguments,
		std::wstring pixelShader
	) noexcept {
		std::string inputName = ModelInputs::GetName<T>(arguments);

		if (!FindModelInput(inputName))
			AddModelInput(std::make_unique<T>(arguments), inputName);

		_addModel(std::move(model), inputName, pixelShader);
	}

	void MoveData() noexcept;

private:
	[[nodiscard]]
	bool FindModelInput(const std::string& name) const noexcept;

	void AddModelInput(
		std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
	) noexcept;
	void AddModelData(
		std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
	) noexcept;
	void AddModelDataMeshlet(
		std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
	) noexcept;

	void AddToGVertices(const std::vector<Vertex>& vertices) noexcept;

	void _addModel(
		std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
	) noexcept;
	void _addModelModelData(
		std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
	) noexcept;
	void _addModelMeshlets(
		std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
	) noexcept;

	void _moveDataModels() noexcept;
	void _moveDataMeshletModels() noexcept;

private:
	std::unordered_map<std::wstring, std::vector<std::shared_ptr<IModel>>> m_modelSets;
	std::unordered_map<std::wstring, std::vector<MeshletModel>> m_meshletModelSets;
	std::vector<Vertex> m_gVertices;
	std::vector<std::uint32_t> m_gVerticesIndices;
	std::vector<std::uint32_t> m_gIndices;
	std::unordered_map<std::string, IndexData> m_modelData;
	std::unordered_map<std::string, std::vector<Meshlet>> m_modelDataMeshlets;
	bool m_processingModeMeshlet;
};
#endif
