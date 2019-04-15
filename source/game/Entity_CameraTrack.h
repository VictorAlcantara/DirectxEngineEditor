#pragma once

#include "../engine/Entity.h"

class Entity_CameraTrack : public Entity
{
	CLASS_TAG( Entity_CameraTrack );

	public:
		Entity_CameraTrack();
		virtual ~Entity_CameraTrack();

		void UpdateCamera(Handle<Entity> hTarget, Handle<Entity> hCamera);

		void ProcessCommand(const ScriptLine*)override final;

		uint GetTrackId()const { return m_trackId; }

	private:
		void AssignNewTrackId(uint trackId);

		static uint s_trackIdControl;
		math::Vec3 m_point0;
		math::Vec3 m_point1;
		uint m_trackId;
};

