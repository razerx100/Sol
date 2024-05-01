#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <vector>
#include <memory>
#include <string>
#include <memory>

#include <ModelProcessor.hpp>

class ModelContainer {
public:
	void SetResources();
	void PhysicsUpdate() noexcept;

	template<DerivedWithoutArgs<ModelInputs> T>
	void AddModel(
		std::shared_ptr<Model>&& model, const std::wstring& pixelShader
	) noexcept {
		m_models.emplace_back(model);

		GetModelProcessor()->AddModel<T>(std::move(model), pixelShader);
	}

	template<DerivedWithArgs<ModelInputs> T>
	void AddModel(
		std::shared_ptr<Model>&& model, const typename T::Args& arguments,
		const std::wstring& pixelShader
	) noexcept {
		m_models.emplace_back(model);

		GetModelProcessor()->AddModel<T>(std::move(model), arguments, pixelShader);
	}

private:
	[[nodiscard]]
	ModelProcessor* GetModelProcessor() const noexcept;

private:
	std::vector<std::shared_ptr<Model>> m_models;
};
#endif
