#ifndef __KEYBOARD_HPP__
#define __KEYBOARD_HPP__
#include <optional>

enum SKeyCodes {
	Default,
	BackSpace,
	Tab,
	Enter,
	Shift,
	Ctrl,
	Alt,
	CapsLock,
	Esc,
	SpaceBar,
	PageUp,
	PageDown,
	End,
	Home,
	LeftArrow,
	UpArrow,
	RightArrow,
	DownArrow,
	PrintScreen,
	Ins,
	Del,
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Super,
	ZeroNumpad, OneNumpad, TwoNumpad, ThreeNumpad, FourNumpad, FiveNumpad, SixNumpad,
	SevenNumpad, EightNumpad, NineNumpad,
	Multiply, Divide, Add, Subtract, Decimal,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	NumLock, ScrollLock,
	ShiftLeft, ShiftRight,
	CtrlLeft, CtrlRight,
	AltLeft, AltRight,
	SemiColonUS,
	Plus,
	Comma,
	Dash,
	Point,
	SlashUS,
	TildeUS,
	BraceStartUS, BraceEndUS,
	BackSlashUS,
	QuoteUS
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
