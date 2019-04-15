#pragma once

#include "../shared/VecGrid.h"
#include "../engine/Entity.h"
#include "../engine/Math.h"
#include "../engine/Pointer.h"
#include "EditorEntityUtil.h"
#include "../engine/Handle.h"
#include "../engine/String.h"

class Model;

/****************************************************************************************
	EditorEntity
*****************************************************************************************/

class EditorEntity : public Entity
{
	CLASS_TAG( EditorEntity );

	public:
		EditorEntity(bool mustUpdate);
		virtual ~EditorEntity();
		
		void SetGridPos(int x, int y, int z, bool allowAdjust = true);
		void SetGridPos(const VecGrid&, bool allowAdjust = true);
		void AddGridPos(int x, int y, int z);
		void AddGridPos(const VecGrid&);
		void SetGridScale(int x, int y, int z);
		void SetGridScale(const VecGrid&);
		void AddGridScale(int, math::eAxis, math::eSign);

		virtual void RotateYawPitchRoll(float yaw, float pitch, float roll) {}
		
		math::Vec3 GetPos()const;
		VecGrid GetGridPos()const;
		math::Vec3 GetScale()const;
		VecGrid GetGridScale()const;
		math::Aabb3 GetAabb()const;
		math::Aabb3 GetAabbGlobal()const;

		uint GetToolActionFlag()const { return m_toolActionFlag; }
		virtual eEditorEntityType GetType()const = 0;

		Handle<Model> GetModel();

		virtual void OnPickEntity(Handle<Entity>);
		virtual void OnToolUse() {}
		virtual bool CanDelete()const { return true; }
		virtual bool CanClone()const { return true; }
		virtual void OnClone() {}

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;
		virtual void OnInitialize_EditorEntity() {}
		virtual void OnShutdown_EditorEntity() {}

		bool ProcessCommand_GridPosScale(const ScriptLine*);
		bool ProcessCommand_Name(const ScriptLine*);

		uint m_toolActionFlag;
		bool m_adjustPosToPair;

	private:
		void AlignGridPosToPair();
		void AdjustTransform();

		Handle<Model> m_hModel;

		VecGrid m_gridPos;
		VecGrid m_gridScale;
};
