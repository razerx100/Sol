#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <concepts>

#include <Model.hpp>

class ModelProcessor {
	struct InputData {
		std::uint32_t indexCount;
		std::uint32_t indexOffset;
		ModelBoundingBox boundingBox;
	};

public:
	ModelProcessor() noexcept;

	template<std::derived_from<ModelInputs> T>
	void AddModel(
		std::shared_ptr<Model> model, std::wstring pixelShader
	) noexcept {
		std::string inputName = ModelInputs::GetName<T>();

		if (!FindModelInput(inputName))
			AddModelInput(std::make_unique<T>(), inputName);

		_addModel(std::move(model), inputName, pixelShader);
	}

	void Process() noexcept;

private:
	[[nodiscard]]
	bool FindModelInput(const std::string& name) const noexcept;

	void AddModelInput(
		std::unique_ptr<ModelInputs> modelInputs, const std::string& inputName
	) noexcept;
	void _addModel(
		std::shared_ptr<Model> model, const std::string& inputName, std::wstring pixelShader
	) noexcept;

private:
	std::vector<std::unique_ptr<ModelInputs>> m_modelInputs;
	std::unordered_map<std::string, InputData> m_modelInputTypes;
	std::unordered_map<std::wstring, std::vector<std::shared_ptr<IModel>>> m_modelSets;
	std::uint32_t m_indexOffset;
};
#endif
