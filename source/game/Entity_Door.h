#pragma once

#include "../engine/Entity.h"
#include "../engine/Gradient.h"
#include "../engine/Component_Collision.h"

struct Debug_Sphere;
struct Debug_Line;

class Entity_Door : public Entity
{
	CLASS_TAG( Entity_Door );

	private:
		enum class eDoorState
		{
			Closing,
			Closed,
			Opening,
			Opened,
		};

	public:
		Entity_Door();
		virtual ~Entity_Door() = default;

		void ProcessCommand(const ScriptLine*)override final;

		void SetDuration(float d) { m_duration = d; }
		void Open();
		void Close();

		const String& GetLockText()const { return m_lockText; }

		bool IsLocked()const { return m_locked; }

	protected:
		void OnInitialize()override final;
		void Update()override final;

	private:
		void UpdateRenderInfo();

		eDoorState m_state;
		HandleAs<Component, Component_Collision> m_hCollision;
		math::Vec3 m_pos;
		float m_duration;
		math::Vec3 m_openDirection;
		math::Vec3 m_closedPos;
		math::Vec3 m_openedPos;
		float m_openAmount;
		bool m_locked;

		String m_lockText;

		float m_closeTimer;
};

