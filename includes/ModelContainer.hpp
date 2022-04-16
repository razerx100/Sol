#ifndef MODEL_CONTAINER_HPP_
#define MODEL_CONTAINER_HPP_
#include <Model.hpp>
#include <memory>
#include <vector>

class ModelContainer {
public:
	Model* AddModel(std::unique_ptr<Model> model);

	void UpdateUVCoordinates() noexcept;
	void ClearModelBuffers() noexcept;

private:
	std::vector<std::unique_ptr<Model>> m_models;
};
#endif
