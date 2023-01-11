#ifndef BASIC_MODEL_INPUTS_HPP_
#define BASIC_MODEL_INPUTS_HPP_
#include <optional>

#include <Model.hpp>

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

class SphereInputs final : public ModelInputs {
public:
	struct Args {
		std::optional<std::uint32_t> latDiv;
		std::optional<std::uint32_t> longDiv;
	};

public:
	SphereInputs(const Args& arguments) noexcept;

	void InitData() noexcept override;

private:
	std::uint32_t m_latitudeDivision;
	std::uint32_t m_longitudeDivision;
};
#endif
