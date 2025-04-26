#ifndef SOL_EXCEPTION_HPP_
#define SOL_EXCEPTION_HPP_
#include <exception>
#include <string>

namespace Sol
{
class Exception : public std::exception
{
public:
	Exception(std::string errorType, std::string errorMessage)
		: m_exceptionType{ std::move(errorType) }, m_errorMessage{ std::move(errorMessage) }
	{}

	[[nodiscard]]
	const char* GetType() const noexcept { return m_exceptionType.c_str(); }
	[[nodiscard]]
	const char* what() const noexcept override { return m_exceptionType.c_str(); }

private:
	std::string m_exceptionType;
	std::string m_errorMessage;
};
}
#endif
