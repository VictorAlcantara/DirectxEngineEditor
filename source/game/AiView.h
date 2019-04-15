#pragma once

#include "../engine/Class.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Table.h"

class Entity;
class AiState;

/****************************************************************************************
	AiView
*****************************************************************************************/

class AiView : public NonCopyable
{
	public:
		AiView() = delete;
		AiView(Handle<Entity>);
		virtual ~AiView() = default;

		virtual void Update() = 0;

		virtual void MoveTo(const math::Vec3&) = 0;
		virtual void Throw(Handle<Entity>&, const math::Vec3& startPos, const math::Vec3& velocity) = 0;

		virtual void OnStateChange(AiState*) {}

		math::Vec3 GetPos()const;
		const math::Mtx44& GetOrientation()const;

		void SetTarget(Handle<Entity>);
		Handle<Entity> GetTarget();

		void SetData(const HashString&, float);
		float GetData(const HashString&, float valueIfNotFound = 0.0f)const;

	protected:
		Handle<Entity> m_hOwner;
		Handle<Entity> m_hTarget;
		Table<float, HashString> m_tableData;
};

/****************************************************************************************
	AiView_Monster
*****************************************************************************************/

class AiView_Monster : public AiView
{
	private:
		enum class eAction
		{
			Idle,
			MoveTo,
			Throw,
		};

	public:
		AiView_Monster() = delete;
		AiView_Monster(Handle<Entity>);
		virtual ~AiView_Monster() = default;

		void Update()override final;

		void MoveTo(const math::Vec3&);
		void Throw(Handle<Entity>&, const math::Vec3& startPos, const math::Vec3& velocity);

		void OnStateChange(AiState*)override final;

	private:
		void Update_MoveTo();

		eAction m_action;
		math::Vec3 m_targetPos;
};
