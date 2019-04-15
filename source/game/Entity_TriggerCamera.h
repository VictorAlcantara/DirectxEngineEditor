#pragma once

#include "../engine/Entity.h"
#include "../engine/Process.h"
#include "../engine/CollisionInfo.h"
#include "../engine/Component_Trigger.h"
#include "../engine/Component_Physics.h"

class Process_CameraTrackFind;
class Entity_CameraTrack;
struct Debug_Cube;

/****************************************************************************************
	Component_Trigger_Camera
*****************************************************************************************/

class Component_Trigger_Camera : public Component_Trigger
{
	CLASS_TAG( Component_Trigger_Camera );

	public:
		Component_Trigger_Camera();
		virtual ~Component_Trigger_Camera() = default;

		void OnTrigger_Enter(Handle<Entity>&)override final;
};

/****************************************************************************************
	Entity_TriggerCamera
*****************************************************************************************/

class Entity_TriggerCamera : public Entity
{
	CLASS_TAG( Entity_TriggerCamera );

	public:
		Entity_TriggerCamera();
		virtual ~Entity_TriggerCamera() = default;

		void ConnectToTrack(Handle<Entity>);
		Handle<Entity> GetCameraTrack();
		Entity_CameraTrack* GetCameraTrackPtr();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize()override final;

	private:
		Handle<Entity> m_hCameraTrack;
		SmartPtr<Process_CameraTrackFind> m_pProcessTrackFind;
		math::Vec3 m_pos;
		math::Vec3 m_scale;
		HandleAs<Component, Component_Trigger_Camera> m_hTrigger;
		HandleAs<Component, Component_Physics> m_hPhysics;
};

/****************************************************************************************
	Process_CameraTrackFind
*****************************************************************************************/

class Process_CameraTrackFind : public Process
{
	public:
		Process_CameraTrackFind();
		virtual ~Process_CameraTrackFind() = default;

		void SetTrackIdToFind(uint trackId);
		void SetOwner(Handle<Entity>);

	protected:
		void OnEnter()override final;
		void Update()override final;

	private:
		Handle<Entity> m_hOwner;
		uint m_trackId;
};
