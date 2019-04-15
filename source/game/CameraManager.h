#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "Entity_GameCamera.h"
#include "../engine/EventListener.h"
#include "GameEnum.h"

class CameraManager : public NonCopyable, public EventListener
{
	public:
		CameraManager();
		~CameraManager();

		void SetMode(eCameraMode mode) { m_mode = mode; }

		void Initialize();
		void Shutdown();

		void SetTarget(Handle<Entity> m_hTarget);
		void Update();
		void HandleEvent(Event*)override final;

		const Entity_GameCamera* GetActiveCamera()const;

	private:
		void UpdateMode_Normal();
		void UpdateMode_Back();

		eCameraMode m_mode;
		Handle<Entity> m_hCamera;
		Handle<Entity> m_hTarget;
		Handle<Entity> m_hTrigger;
};
