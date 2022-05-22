#ifndef TIME_MANAGER_HPP_
#define TIME_MANAGER_HPP_
#include <chrono>

class Timer {
public:
	void SetTimer() noexcept;

	[[nodiscard]]
	std::int64_t GetTimeNano() const noexcept;
	[[nodiscard]]
	std::int64_t GetTimeMicro() const noexcept;
	[[nodiscard]]
	std::int64_t GetTimeMili() const noexcept;
	[[nodiscard]]
	std::int64_t GetTimeSecond() const noexcept;

	[[nodiscard]]
	std::chrono::nanoseconds GetDurationNano() const noexcept;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>
		m_timePoint;
};

class FrameTime {
public:
	FrameTime() noexcept;

	void StartTimer() noexcept;
	void EndTimer() noexcept;
	void SetGraphicsUpdateDelta(double delta) noexcept;

	[[nodiscard]]
	double GetGraphicsUpdateDelta() const noexcept;
	[[nodiscard]]
	double GetDeltaTime() const noexcept;

private:
	Timer m_timer;
	double m_deltaTime;
	double m_graphicsUpdateDelta;
};

#endif
