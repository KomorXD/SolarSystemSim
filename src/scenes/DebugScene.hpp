#include "Scene.hpp"

class DebugScene : public Scene
{
public:
	DebugScene();
	~DebugScene();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts)	override;
	virtual void OnRender()			override;

private:
	bool m_ShouldBeRendered = false;
	float m_TimePassedInSeconds = 0.0f;
};