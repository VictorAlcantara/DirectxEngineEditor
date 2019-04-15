#pragma once

#include "../engine/Entity.h"
#include "../engine/Process.h"
#include "../engine/CollisionInfo.h"
#include "../engine/ScriptReader.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"

class Entity_Talk : public Entity
{
	CLASS_TAG( Entity_Talk );

	public:
		Entity_Talk();
		virtual ~Entity_Talk() = default;

		void ProcessCommand(const ScriptLine*)override final;

		const List<String>& GetTalkLines()const;
		const List<String>& GetCommandLines(bool increaseEventCounter)const;

		bool CanFireEvent()const { return !m_eventFireOnce || m_eventFireCount < 1; }

	protected:
		void OnInitialize()override final;

	private:
		math::Vec3 m_pos;
		math::Vec3 m_scale;

		HandleAs<Component, Component_Collision> m_hCollision;
		HandleAs<Component, Component_Physics> m_hPhysics;

		List<String> m_talkLines;
		List<String> m_cmdLines;
		bool m_eventFireOnce;
		mutable uint m_eventFireCount;
};
