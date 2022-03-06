#ifndef __BASIC_MODELS_HPP__
#define __BASIC_MODELS_HPP__
#include <Model.hpp>

class Triangle : public Model {
public:
	Triangle() noexcept;
	Triangle(const Ceres::Float32_4& solidColour) noexcept;

private:
	void Init(const Ceres::Float32_4& solidColour) noexcept;
};

class Triangle1 : public Model {
public:
	Triangle1() noexcept;
	Triangle1(const Ceres::Float32_4& solidColour) noexcept;

private:
	void Init(const Ceres::Float32_4& solidColour) noexcept;
};
#endif
