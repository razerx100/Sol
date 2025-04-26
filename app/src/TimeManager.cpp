#include <TimeManager.hpp>

namespace Sol
{
// Frame Time
void FrameTime::EndTimer() noexcept
{
	m_deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(
		m_timer.GetDurationNano()
		).count();

	m_oneSecond += m_deltaTime;

	++m_frameCount;
}

void FrameTime::ResetFrameCount() noexcept
{
	m_frameCount = 0u;
	m_oneSecond  = 0.f;
}
}
