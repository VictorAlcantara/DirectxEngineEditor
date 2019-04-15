//#pragma once
//
//#include "../engine/Entity.h"
//#include "../engine/Math.h"
//#include "DebugRender.h"
//#include "CharacterControl.h"
//#include "../engine/Component_Collision.h"
//
//class Entity_Player;
//
//class Entity_Test : public Entity
//{
//	friend class Entity_Player;
//
//	CLASS_TAG( Entity_Test );
//
//	public:
//		Entity_Test();
//		virtual ~Entity_Test();
//
//		void ProcessCommand(const ScriptLine*)override final;
//		void OnPhysicsUpdate(Contact*, uint contactCount)override final;
//
//	protected:
//		void Update()override final;
//		void UpdateControl();
//		void Initialize()override final;
//		void Shutdown()override final;
//
//	private:
//		void SetScale(const math::Vec3&);
//		void UseGravity(bool);
//
//		CollisionInfo_Ellipse* m_pCollision;
//		float m_movementForce;
//		math::Mtx44 m_orientation;
//
//		bool m_grounded;
//		SimplePtr<CharacterControl> m_pCharacterControl;
//};
