#pragma once

#include <functional>
#include <chrono>
#include <vector>

class Application;

class TriggerClock
{
public:
	friend class Application;

	TriggerClock(std::function<void(void)> f = []() {});
	~TriggerClock();

	void Start();
	void Stop();
	void Restart();

	void SetInterval(uint32_t interval);
	void SetFunction(std::function<void(void)> f);

private:
	std::function<void(void)> m_Func;
	float m_PassedTime = 0.0f;
	float m_Interval = 0.0f;
	bool m_Active = false;

	void Update(float ts);

	static void Init();
	static void UpdateClocks();

	static std::vector<TriggerClock*> m_Clocks;
	static std::chrono::system_clock::time_point m_PreviousTimepoint;
};