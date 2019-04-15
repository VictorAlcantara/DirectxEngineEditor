#pragma once

#include "../engine/Entity.h"
#include "../engine/Math.h"
#include "CharacterControl.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"
#include "../engine/Component_Render.h"

class Entity_Test;
struct Debug_Line;

/****************************************************************************************
	Entity_Player
*****************************************************************************************/

class Entity_Player : public Entity
{
	CLASS_TAG( Entity_Player );

	public:
		Entity_Player();
		virtual ~Entity_Player() = default;

		math::Vec3 GetLook()const;

		void ProcessCommand(const ScriptLine*)override final;

		void TakeDamage(const HashString& damageType);

		math::CameraTransform GetCameraTransform()const;

		void Active(bool b);

	protected:
		void Update()override final;
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateViewRotation();
		void UpdateCharacterControl();
		void UpdateRenderTransformInfo();
		void UpdateRenderRotationInfo();

		void SetScale(const math::Vec3&);

		HandleAs<Component, Component_Collision> m_hCollision;
		HandleAs<Component, Component_Physics> m_hPhysics;
		HandleAs<Component, Component_Render> m_hRender;

		float m_movementForce;
		float m_rotationSpeed;
		math::Mtx44 m_orientation;
		math::Euler m_angle;
		math::Vec3 m_cameraRelativePos;
		Handle<Entity> m_hTest;

		float m_damageTimeEnd;
		Handle<Debug_Line> m_hDebugDirection;

		bool m_active;
};
