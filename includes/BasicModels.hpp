#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <memory>
#include <concepts>

#include <Model.hpp>
#include <Sol.hpp>

class OneThirdModel : public Model {
public:
	OneThirdModel() noexcept;

private:
	void InitData() noexcept;
};

class TriangleInputs final : public ModelInputs {
public:
	TriangleInputs() noexcept;

	void InitData() noexcept override;
};

class CubeInputs final : public ModelInputs {
public:
	CubeInputs() noexcept;

	void InitData() noexcept override;
};

class QuadInputs final : public ModelInputs {
public:
	QuadInputs() noexcept;

	void InitData() noexcept override;
};

#endif
