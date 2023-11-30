#pragma once

#include "Components.hpp"

#include <string>

enum class ObjectType
{
	Empty,
	DirectionalLight,
	Planet,
	Sun
};

class SceneObject
{
public:
	SceneObject()
	{
		m_ObjectID = s_IdCounter;
		m_Tag.reserve(24);
		m_Tag = std::string("Object #") + std::to_string(m_ObjectID);
		++s_IdCounter;
	}

	virtual ~SceneObject() = default;

	virtual void OnUpdate(float ts) = 0;
	virtual void OnTick() = 0;
	virtual void OnConfigRender() = 0;

	inline ObjectType GetType()   const { return m_Type;	 }
	inline std::string GetTag()   const { return m_Tag;		 }
	inline uint32_t GetEntityID() const { return m_ObjectID; }
	
	inline Transform& GetTransform() { return m_Transform; }

	inline static constexpr uint32_t MAX_OBJECTS = 254;

	static void CleanLastNIDs(int32_t N) { s_IdCounter -= N; }

protected:
	std::string m_Tag;
	Transform m_Transform;
	uint32_t m_ObjectID = 0;

	ObjectType m_Type = ObjectType::Empty;

	static inline uint32_t s_IdCounter = 1;

	friend class SceneSerializer;
};