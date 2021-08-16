#ifndef __MOUSE_HPP__
#define __MOUSE_HPP__
#include <optional>

class Mouse {
public:
	struct RawDelta {
		int x;
		int y;
	};

	struct MouseData {
		bool leftPressed;
		bool rightPressed;
		bool middlePressed;
		int x;
		int y;
	};

	class Event {
	public:
		enum class Type {
			LPress,
			LRelease,
			MPress,
			MRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type m_type;
		MouseData m_data;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_data{} {}

		Event(Type type, const MouseData& data) noexcept
			: m_type(type),
			m_data(data) {}

		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept {
			return m_type;
		}

		std::pair<int, int> GetPos() const noexcept {
			return { m_data.x, m_data.y };
		}

		int GetPosX() const noexcept {
			return m_data.x;
		}

		int GetPosY() const noexcept {
			return m_data.y;
		}

		bool IsLeftPressed() const noexcept {
			return m_data.leftPressed;
		}

		bool IsMiddlePressed() const noexcept {
			return m_data.middlePressed;
		}

		bool IsRightPressed() const noexcept {
			return m_data.rightPressed;
		}
	};

public:
	virtual ~Mouse() = default;

	virtual std::pair<int, int> GetPos() const noexcept = 0;
	virtual int GetPosX() const noexcept = 0;
	virtual int GetPosY() const noexcept = 0;
	virtual std::optional<Event> Read() noexcept = 0;
	virtual std::optional<RawDelta> ReadRawDelta() noexcept = 0;

	virtual bool IsInWindow() const noexcept = 0;
	virtual bool IsLeftPressed() const noexcept = 0;
	virtual bool IsMiddlePressed() const noexcept = 0;
	virtual bool IsRightPressed() const noexcept = 0;
	virtual bool IsBufferEmpty() const noexcept = 0;
	virtual bool IsRawEnabled() const noexcept = 0;

	virtual void EnableRaw() noexcept = 0;
	virtual void DisableRaw() noexcept = 0;
};
#endif
