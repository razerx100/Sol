#ifndef __BASIC_MODELS_HPP__
#define __BASIC_MODELS_HPP__
#include <Model.hpp>

class Triangle : public Model {
public:
	Triangle() noexcept;

private:
	void Init() noexcept;
};

class Triangle1 : public Model {
public:
	Triangle1() noexcept;

private:
	void Init() noexcept;
};
#endif
