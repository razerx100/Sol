#ifndef BASIC_MODEL_INPUTS_HPP_
#define BASIC_MODEL_INPUTS_HPP_
#include <optional>

#include <ModelBase.hpp>

class TriangleInputs final : public ModelInputs {
public:
	TriangleInputs() noexcept;

	void InitData() noexcept override;
};

enum class CubeUVMode {
	SingleColour,
	IndependentFaceTexture
};

class CubeInputs final : public ModelInputs {
public:
	struct Args {
		std::optional<CubeUVMode> uvMode;
	};

public:
	CubeInputs(const Args& args) noexcept;

	void InitData() noexcept override;

private:
	void SetSingleColourUV() noexcept;
	void SetIndependentFaceTexUV() noexcept;

private:
	CubeUVMode m_uvMode;
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
	void CalculateNormals() noexcept;

private:
	std::uint32_t m_latitudeDivision;
	std::uint32_t m_longitudeDivision;
};
#endif
