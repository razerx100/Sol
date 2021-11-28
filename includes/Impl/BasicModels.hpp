#ifndef __BASIC_MODELS_HPP__
#define __BASIC_MODELS_HPP__
#include <Model.hpp>

class Triangle : public Model {
public:
	Triangle() noexcept;
	Triangle(const Ceres::VectorF32& solidColor) noexcept;

private:
	void Init(const Ceres::VectorF32& solidColor) noexcept;
};
#endif
