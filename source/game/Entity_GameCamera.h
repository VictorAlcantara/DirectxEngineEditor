#pragma once

#include "../engine/Entity.h"

class Entity_GameCamera : public Entity
{
	CLASS_TAG( Entity_GameCamera );

	public:
		Entity_GameCamera();
		virtual ~Entity_GameCamera();

		void SetPos(const math::Vec3& pos);
		void LookAt(const math::Vec3& targetPos);
		math::CameraTransform GetCameraTransform()const;
		
		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		math::CameraTransform m_transform;
};


