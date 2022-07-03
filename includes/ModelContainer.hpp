#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <memory>
#include <vector>

#include <Model.hpp>

class ModelContainer {
public:
	void AddModel(std::shared_ptr<Model> model);

	void SubmitModelsToRenderer() const noexcept;
	void ClearModelBuffers() noexcept;

private:
	std::vector<std::shared_ptr<Model>> m_models;
};
#endif
