#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <memory>
#include <concepts>
#include <string>
#include <optional>

#include <Model.hpp>
#include <Sol.hpp>

class OneThirdModel : public Model {
public:
	OneThirdModel() noexcept;

	virtual void SetResources() override;

	void SetTextureName(const std::string& texName) noexcept;

private:
	void InitData() noexcept;

private:
	std::string m_textureName;
};

class ModelWithArrow : public OneThirdModel {
public:
	virtual void PhysicsUpdate() noexcept override;
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
