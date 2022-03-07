#ifndef __MODEL_CONTAINER_HPP__
#define __MODEL_CONTAINER_HPP__
#include <IModelContainer.hpp>
#include <vector>

class ModelContainer : public IModelContainer {
public:
	ISolModel* AddModel(std::unique_ptr<ISolModel> model) override;

	void UpdateUVCoordinates() noexcept override;
	void ClearModelBuffers() noexcept override;

private:
	std::vector<std::unique_ptr<ISolModel>> m_models;
};
#endif
