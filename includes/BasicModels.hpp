#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
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
