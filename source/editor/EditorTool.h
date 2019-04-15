#pragma once

#include "../engine/Math.h"
#include "../engine/Pointer.h"
#include "../engine/Entity.h"
#include "../engine/Handle.h"
#include "../engine/Model.h"
#include "RenderInfoEditor.h"

class EditorEntity;
class Shader_ColorLightDir;
class Shader_OneColor;
class IEditorToolAction;

enum class eEditorToolAction
{
	None,
	Translate,
	Resize,
	Rotate,
	PickAxis,
};

struct EditorToolActionInfo
{
	math::eAxis axisEnum;
	Handle<Entity> hEntity;
	math::Vec3 toolCollisionPoint;
	math::CameraTransform camera;

	EditorToolActionInfo() : axisEnum(math::eAxis::X) {}
};

/****************************************************************************************
	EditorTool
*****************************************************************************************/

class EditorTool
{
	public:
		EditorTool();
		~EditorTool();

		void Initialize(math::eAxis);
		void Shutdown();
		void Update();

		void PrepareUse(const math::Vec3& toolCollisionPoint, eEditorToolAction);
		bool Use(const math::Vec2& movementDist);
		void EndUse();

		void Attach(Handle<Entity>);
		void SetCameraTransform(const math::CameraTransform&);

		float GetClosestCollision(const math::Vec3& rayOrigin, const math::Vec3& rayDir);

		const RenderInfo_EditorTool& GetRenderInfo()const { return m_renderInfo; }

		bool IsUsing()const { return m_using; }

	private:
		void CalculateScaleRelativeToCamera(Handle<Entity>, const math::CameraTransform&);
		Handle<Model> CreateModel(uint slice);

		bool m_initialized;
		Handle<Entity> m_hEntityAttached;
		math::CameraTransform m_cameraTransform;
		RenderInfo_EditorTool m_renderInfo;

		SimplePtr<IEditorToolAction> m_pAction;

		math::eAxis m_axisEnum;
		math::Vec3 m_axis;
		math::Vec3 m_pos;
		math::Vec3 m_scaleBase;
		math::Vec3 m_scale;
		math::Mtx44 m_rotation;

		bool m_using;
};

/****************************************************************************************
	IEditorToolAction
*****************************************************************************************/

class IEditorToolAction
{
	public:
		virtual void PrepareUse(const EditorToolActionInfo&) = 0;
		virtual bool Use(Handle<Entity>, const math::Vec2& dist, const math::CameraTransform& camera) = 0;
		virtual eEditorToolAction GetActionEnum()const = 0;
};

/****************************************************************************************
	EditorToolAction_Base
*****************************************************************************************/

class EditorToolAction_Base : public IEditorToolAction
{
	public:
		EditorToolAction_Base() : m_axisEnum(math::eAxis::X), m_totalMovement(0.0f) {}
		virtual ~EditorToolAction_Base() = default;

		void PrepareUse(const EditorToolActionInfo&)override final;

	protected:
		virtual void OnPrepareUse(const EditorToolActionInfo&) {}
		virtual float CalculateAmount(const math::Vec2& dist, const math::CameraTransform& camera);
		math::Vec3 GetAxis(bool isSigned = false)const;
		float GetGridLeveling()const;

		math::eAxis m_axisEnum;
		math::Vec3 m_axis;
		math::eSign m_axisSign;
		float m_totalMovement;
};

/****************************************************************************************
	EditorToolAction_Resize
*****************************************************************************************/

class EditorToolAction_Resize : public EditorToolAction_Base
{
	public:
		virtual ~EditorToolAction_Resize() = default;

		bool Use(Handle<Entity>, const math::Vec2& dist, const math::CameraTransform& camera)override final;
		eEditorToolAction GetActionEnum()const override final { return eEditorToolAction::Resize; }
};

/****************************************************************************************
	EditorToolAction_Translate
*****************************************************************************************/

class EditorToolAction_Translate : public EditorToolAction_Base
{
	public:
		virtual ~EditorToolAction_Translate() = default;

		bool Use(Handle<Entity>, const math::Vec2& dist, const math::CameraTransform& camera)override final;
		eEditorToolAction GetActionEnum()const override final { return eEditorToolAction::Translate; }
};

/****************************************************************************************
	EditorToolAction_Rotate
*****************************************************************************************/

class EditorToolAction_Rotate : public EditorToolAction_Base
{
	public:
		virtual ~EditorToolAction_Rotate() = default;

		float CalculateAmount(const math::Vec2& dist, const math::CameraTransform& camera)override final;
		bool Use(Handle<Entity>, const math::Vec2& dist, const math::CameraTransform& camera)override final;
		eEditorToolAction GetActionEnum()const override final { return eEditorToolAction::Rotate; }
};

/****************************************************************************************
	EditorToolAction_PickAxis
*****************************************************************************************/

class EditorToolAction_PickAxis : public EditorToolAction_Base
{
	public:
		virtual ~EditorToolAction_PickAxis() = default;

		bool Use(Handle<Entity>, const math::Vec2& dist, const math::CameraTransform& camera)override final;
		eEditorToolAction GetActionEnum()const override final { return eEditorToolAction::PickAxis; }
};
