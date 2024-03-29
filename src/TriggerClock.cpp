#include "TriggerClock.hpp"

std::vector<TriggerClock*> TriggerClock::m_Clocks;
std::chrono::system_clock::time_point TriggerClock::m_PreviousTimepoint = std::chrono::system_clock::now();

TriggerClock::TriggerClock(std::function<void(void)> f)
	: m_Func(f)
{
	m_Clocks.push_back(this);
}

TriggerClock::~TriggerClock()
{
	m_Clocks.erase(std::find(m_Clocks.begin(), m_Clocks.end(), this));
}

void TriggerClock::Start()
{
	m_Active = true;
}

void TriggerClock::Stop()
{
	m_Active = false;
}

void TriggerClock::Restart()
{
	m_Active = true;
	m_PassedTime = 0.0f;
}

void TriggerClock::SetInterval(uint32_t interval)
{
	m_Interval = interval;
}

void TriggerClock::SetFunction(std::function<void(void)> f)
{
	m_Func = f;
}

void TriggerClock::Update(float ts)
{
	if (!m_Active)
	{
		return;
	}

	m_PassedTime += ts;

	if (m_PassedTime >= m_Interval)
	{
		m_Func();
		m_PassedTime = m_PassedTime - m_Interval;
	}
}

void TriggerClock::UpdateClocks()
{
	auto currentTP = std::chrono::system_clock::now();
	float ts = std::chrono::duration<float>(currentTP - m_PreviousTimepoint).count() * 1000.0f;

	m_PreviousTimepoint = currentTP;

	for (auto& clock : m_Clocks)
	{
		clock->Update(ts);
	}
}
