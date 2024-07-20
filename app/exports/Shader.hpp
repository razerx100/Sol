#ifndef SHADER_SOL_HPP_
#define SHADER_SOL_HPP_
#include <string>
#include <compare>

enum class ShaderType
{
	SPIRV,
	DXIL
};

class ShaderName
{
public:
	ShaderName() : m_name{} {}
	ShaderName(const std::wstring& name) : m_name{ name } {};
	ShaderName(const wchar_t* name) : m_name{ name } {};

	void SetName(const std::wstring& name) noexcept { m_name = name; }

	[[nodiscard]]
	std::wstring GetName() const noexcept { return m_name; }
	[[nodiscard]]
	std::wstring GetNameWithExtension(ShaderType type) const noexcept
	{
		std::wstring extension{};

		if (type == ShaderType::SPIRV)
			extension = L".spv";
		else if (type == ShaderType::DXIL)
			extension = L".cso";

		return m_name + extension;
	}

	bool empty() const noexcept { return std::empty(m_name); }

private:
	std::wstring m_name;

public:
	ShaderName(const ShaderName& other) noexcept : m_name{ other.m_name } {}
	ShaderName& operator=(const ShaderName& other) noexcept
	{
		m_name = other.m_name;

		return *this;
	}

	ShaderName(ShaderName&& other) noexcept : m_name{ std::move(other.m_name) } {}
	ShaderName& operator=(ShaderName&& other) noexcept
	{
		m_name = std::move(other.m_name);

		return *this;
	}

	bool operator==(const ShaderName& other) const noexcept
	{
		return m_name == other.m_name;
	}
};
#endif
