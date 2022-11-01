#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_
#include <exception>
#include <string>

class Exception : public std::exception {
public:
	Exception(const std::string& errorType, const std::string& errorMessage) noexcept;

	[[nodiscard]]
	const char* GetType() const noexcept;
	[[nodiscard]]
	const char* what() const noexcept override;

private:
	std::string m_exceptionType;
	std::string m_errorMessage;
};
#endif
