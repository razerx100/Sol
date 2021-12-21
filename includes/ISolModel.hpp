#ifndef __I_SOL_MODEL_HPP__
#define __I_SOL_MODEL_HPP__
#include <IModel.hpp>

class ISolModel : public IModel {
public:
	virtual ~ISolModel() = default;

	virtual void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept = 0;
	virtual void ResetVerticesAndIndices() noexcept = 0;
};
#endif
