#include<Keyboard.hpp>

bool Keyboard::IsKeyPressed(SKeyCodes keycode) const noexcept {
	return m_keystates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept {
	if (!m_keyBuffer.empty()) {
		Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	else
		return std::nullopt;
}

bool Keyboard::IsKeyEmpty() const noexcept {
	return m_keyBuffer.empty();
}

std::optional<char> Keyboard::ReadChar() noexcept {
	if (!m_charBuffer.empty()) {
		unsigned char charCode = m_charBuffer.front();
		m_charBuffer.pop();
		return charCode;
	}
	else
		return std::nullopt;
}

bool Keyboard::IsCharEmpty() const noexcept {
	return m_charBuffer.empty();
}

void Keyboard::FlushChar() noexcept {
	m_charBuffer = std::queue<char>();
}

void Keyboard::FlushKey() noexcept {
	m_keyBuffer = std::queue<Event>();
}

void Keyboard::Flush() noexcept {
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept {
	m_autoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept {
	m_autoRepeatEnabled = false;
}

bool Keyboard::IsAutoRepeatEnabled() const noexcept {
	return m_autoRepeatEnabled;
}

void Keyboard::OnKeyPressed(std::uint16_t keycode) noexcept {
	SKeyCodes sKeyCode = GetSKeyCodes(keycode);

	m_keystates[sKeyCode] = true;
	m_keyBuffer.emplace(Keyboard::Event(Keyboard::Event::Type::Press, sKeyCode));
	TrimBuffer(m_keyBuffer);
}

void Keyboard::OnKeyReleased(std::uint16_t keycode) noexcept {
	SKeyCodes sKeyCode = GetSKeyCodes(keycode);

	m_keystates[sKeyCode] = false;
	m_keyBuffer.emplace(Keyboard::Event(Keyboard::Event::Type::Release, sKeyCode));
	TrimBuffer(m_keyBuffer);
}

void Keyboard::OnChar(char character) noexcept {
	m_charBuffer.emplace(character);
	TrimBuffer(m_charBuffer);
}

void Keyboard::ClearState() noexcept {
	m_keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > s_bufferSize)
		buffer.pop();
}

void Keyboard::Init() {
	s_instance = std::make_unique<Keyboard>();
}

Keyboard* Keyboard::GetRef() noexcept {
	return s_instance.get();
}
