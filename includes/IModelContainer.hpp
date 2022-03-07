#ifndef __I_MODEL_CONTAINER_HPP__
#define __I_MODEL_CONTAINER_HPP__
#include <ISolModel.hpp>
#include <memory>

class IModelContainer {
public:
	virtual ~IModelContainer() = default;

	virtual ISolModel* AddModel(std::unique_ptr<ISolModel> model) = 0;

	virtual void UpdateUVCoordinates() noexcept = 0;
	virtual void ClearModelBuffers() noexcept = 0;
};

[[nodiscard]]
IModelContainer* CreateModelContainerInstance();
#endif
