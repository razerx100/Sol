#ifndef __KEYBOARD_HPP__
#define __KEYBOARD_HPP__
#include <optional>

// Need to add keycodes
enum SKeyCodes {
	Default
};

class Keyboard {
public:
	class Event {
	public:
		enum class Type {
			Press,
			Release,
			Invalid
		};

	private:
		Type m_type;
		SKeyCodes m_code;

	public:
		Event() noexcept
			: m_type(Type::Invalid), m_code(SKeyCodes::Default) {}
		Event(Type type, SKeyCodes code) noexcept
			: m_type(type), m_code(code) {}
		bool IsPress() const noexcept {
			return m_type == Type::Press;
		}
		bool IsRelease() const noexcept {
			return m_type == Type::Release;
		}
		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}
		SKeyCodes GetCode() const noexcept {
			return m_code;
		}
	};

public:
	virtual ~Keyboard() = default;

	virtual bool IsKeyPressed(SKeyCodes keycode) const noexcept = 0;
	virtual std::optional<Event> ReadKey() noexcept = 0;
	virtual std::optional<char> ReadChar() noexcept = 0;
};
#endif
