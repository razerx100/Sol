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
FrameTime::FrameTime() noexcept
	: m_deltaTime(0), m_graphicsUpdateDelta(0.02), m_oneSecond(0), m_frameCount(0u) {}

void FrameTime::StartTimer() noexcept {
	m_timer.SetTimer();
}

void FrameTime::EndTimer() noexcept {
	m_deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(
		m_timer.GetDurationNano()
		).count();

	m_oneSecond += m_deltaTime;
	++m_frameCount;
}

float FrameTime::GetDeltaTime() const noexcept {
	return m_deltaTime;
}

void FrameTime::SetGraphicsUpdateDelta(float delta) noexcept {
	m_graphicsUpdateDelta = delta;
}

float FrameTime::GetGraphicsUpdateDelta() const noexcept {
	return m_graphicsUpdateDelta;
}

void FrameTime::ResetFrameCount() noexcept {
	m_frameCount = 0u;
	m_oneSecond = 0.f;
}

std::uint64_t FrameTime::GetFrameCount() const noexcept {
	return m_frameCount;
}

bool FrameTime::HasASecondPassed() const noexcept {
	return m_oneSecond >= 1.f;
}
