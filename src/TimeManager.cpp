#include <TimeManager.hpp>

// Timer
void Timer::SetTimer() noexcept {
	m_timePoint = std::chrono::high_resolution_clock::now();
}

std::chrono::nanoseconds Timer::GetDurationNano() const noexcept {
	return std::chrono::high_resolution_clock::now() - m_timePoint;
}

std::int64_t Timer::GetTimeNano() const noexcept {
	return GetDurationNano().count();
}

std::int64_t Timer::GetTimeMicro() const noexcept {
	return std::chrono::duration_cast<std::chrono::microseconds>(GetDurationNano()).count();
}

std::int64_t Timer::GetTimeMili() const noexcept {
	return std::chrono::duration_cast<std::chrono::milliseconds>(GetDurationNano()).count();
}

std::int64_t Timer::GetTimeSecond() const noexcept {
	return std::chrono::duration_cast<std::chrono::seconds>(GetDurationNano()).count();
}

// Frame Time
FrameTime::FrameTime() noexcept : m_deltaTime(0), m_graphicsUpdateDelta(1./144) {}

void FrameTime::StartTimer() noexcept {
	m_timer.SetTimer();
}

void FrameTime::EndTimer() noexcept {
	m_deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(
		m_timer.GetDurationNano()
		).count();
}

double FrameTime::GetDeltaTime() const noexcept {
	return m_deltaTime;
}

void FrameTime::SetGraphicsUpdateDelta(double delta) noexcept {
	m_graphicsUpdateDelta = delta;
}

double FrameTime::GetGraphicsUpdateDelta() const noexcept {
	return m_graphicsUpdateDelta;
}
