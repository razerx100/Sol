#ifndef __I_SOL_MODEL_HPP__
#define __I_SOL_MODEL_HPP__
#include <IModel.hpp>

class ISolModel : public IModel {
public:
	virtual ~ISolModel() = default;

	virtual void ResetVerticesAndIndices() noexcept = 0;
	virtual void SetTextureIndex(size_t index) noexcept = 0;
};
#endif
