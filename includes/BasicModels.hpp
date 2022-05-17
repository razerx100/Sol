#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <Model.hpp>

class Triangle final : public Model {
public:
	Triangle() noexcept;

private:
	void Init() noexcept;
};

class Cube final : public Model {
public:
	Cube() noexcept;

private:
	void Init() noexcept;
};
#endif
