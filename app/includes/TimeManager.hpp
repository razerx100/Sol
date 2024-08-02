#ifndef TIME_MANAGER_HPP_
#define TIME_MANAGER_HPP_
#include <chrono>

class Timer
{
	using Timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;
public:
	Timer() : m_timePoint{} {}

	void SetTimer() noexcept
	{
		m_timePoint = std::chrono::high_resolution_clock::now();
	}

	[[nodiscard]]
	std::int64_t GetTimeNano() const noexcept { return GetDurationNano().count(); }
	[[nodiscard]]
	std::int64_t GetTimeMicro() const noexcept
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(GetDurationNano()).count();
	}
	[[nodiscard]]
	std::int64_t GetTimeMili() const noexcept
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(GetDurationNano()).count();
	}
	[[nodiscard]]
	std::int64_t GetTimeSecond() const noexcept
	{
		return std::chrono::duration_cast<std::chrono::seconds>(GetDurationNano()).count();
	}

	[[nodiscard]]
	std::chrono::nanoseconds GetDurationNano() const noexcept
	{
		return std::chrono::high_resolution_clock::now() - m_timePoint;
	}

private:
	Timepoint_t m_timePoint;

public:
	Timer(const Timer& other) noexcept : m_timePoint{ other.m_timePoint }
	{}
	Timer& operator=(const Timer& other) noexcept
	{
		m_timePoint = other.m_timePoint;

		return *this;
	}

	Timer(Timer&& other) noexcept : m_timePoint{ std::move(other.m_timePoint) }
	{}
	Timer& operator=(Timer&& other) noexcept
	{
		m_timePoint = std::move(other.m_timePoint);

		return *this;
	}
};

class FrameTime
{
public:
	FrameTime()
		: m_timer{}, m_deltaTime{ 0.f }, m_oneSecond{ 0.f }, m_graphicsUpdateDelta{ 0.2f },
		m_frameCount{ 0u }
	{}

	void EndTimer() noexcept;
	void ResetFrameCount() noexcept;
	void SetGraphicsUpdateDelta(float delta) noexcept { m_graphicsUpdateDelta = delta; }

	[[nodiscard]]
	float GetGraphicsUpdateDelta() const noexcept { return m_graphicsUpdateDelta; }

	[[nodiscard]]
	float GetDeltaTime() const noexcept { return m_deltaTime; }
	[[nodiscard]]
	std::uint64_t GetFrameCount() const noexcept { return m_frameCount; }
	[[nodiscard]]
	bool HasASecondPassed() const noexcept { return m_oneSecond >= 1.f; }

	[[nodiscard]]
	const Timer& GetTimer() const noexcept { return m_timer; }
	[[nodiscard]]
	Timer& GetTimer() noexcept { return m_timer; }

private:
	Timer         m_timer;
	float         m_deltaTime;
	float         m_oneSecond;
	float         m_graphicsUpdateDelta;
	std::uint64_t m_frameCount;

public:
	FrameTime(const FrameTime& other) noexcept
		: m_timer{ other.m_timer }, m_deltaTime{ other.m_deltaTime }, m_oneSecond{ other.m_oneSecond },
		m_graphicsUpdateDelta{ other.m_graphicsUpdateDelta }, m_frameCount{ other.m_frameCount }
	{}
	FrameTime& operator=(const FrameTime& other) noexcept
	{
		m_timer               = other.m_timer;
		m_deltaTime           = other.m_deltaTime;
		m_oneSecond           = other.m_oneSecond;
		m_graphicsUpdateDelta = other.m_graphicsUpdateDelta;
		m_frameCount          = other.m_frameCount;

		return *this;
	}
	FrameTime(FrameTime&& other) noexcept
		: m_timer{ std::move(other.m_timer) }, m_deltaTime{ other.m_deltaTime },
		m_oneSecond{ other.m_oneSecond }, m_graphicsUpdateDelta{ other.m_graphicsUpdateDelta },
		m_frameCount{ other.m_frameCount }
	{}
	FrameTime& operator=(FrameTime&& other) noexcept
	{
		m_timer               = std::move(other.m_timer);
		m_deltaTime           = other.m_deltaTime;
		m_oneSecond           = other.m_oneSecond;
		m_graphicsUpdateDelta = other.m_graphicsUpdateDelta;
		m_frameCount          = other.m_frameCount;

		return *this;
	}
};
#endif
