#pragma once

#include "../engine/Math.h"

class FirstPersonCamera
{
	public:
		FirstPersonCamera();
		~FirstPersonCamera();

		void Update();

		void SetRotationSpeed(float);
		void SetSpeed(float);
		void SetPos(float x, float y, float z);
		void SetYawPitchRollDegs(float yaw, float pitch, float roll);
		void SetFov(float fovDegs, float aspectRatio, float _near = 0.01f, float _far = 1000.0f);

		math::Vec3 GetPos()const;
		math::Vec3 GetRight()const;
		math::Vec3 GetUp()const;
		math::Vec3 GetLook()const;

		math::Mtx44 GetProj()const;

		math::CameraTransform GetCameraTransform()const;

	private:
		math::Mtx44 GetRotation()const;

		math::Mtx44 m_proj;
		math::Vec3 m_pos;
		math::Vec3 m_rotationDegs;
		float m_aspectRatio;
		float m_fovDegs;
		float m_near;
		float m_far;

		float m_speed;
		float m_rotationSpeed;
};
