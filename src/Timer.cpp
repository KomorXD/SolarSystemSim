#include "Timer.hpp"
#include "Logger.hpp"

Timer::Timer(const std::string& taskName)
	: m_Name(taskName)
{
	m_T1 = std::chrono::steady_clock::now();

	LOG_INFO("Started {}.", m_Name);
}

Timer::~Timer()
{
	m_T2 = std::chrono::steady_clock::now();

	float result = std::chrono::duration<float>(m_T2 - m_T1).count() * 1000.0f;

	LOG_INFO("{} finished after {:.2f}ms.", m_Name, result);
}