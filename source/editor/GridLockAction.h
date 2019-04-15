#pragma once

#include "GridUtil.h"
#include "../engine/List.h"
#include "../engine/ListIterator.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"

class EditorEntity;

//=============================================================================
//	IGridLockAction
//=============================================================================

class IGridLockAction
{
	public:
		virtual ~IGridLockAction() = default;

		virtual void Lock(const GridLockActionInfo&) = 0;
		virtual void Update(const math::CameraTransform&, float scale) = 0;
		virtual const ListIteratorConst<GridRenderData> GetRenderIterator()const = 0;
};

//=============================================================================
//	GridLockActionBase
//=============================================================================

class GridLockActionBase : public IGridLockAction
{
	public:
		virtual ~GridLockActionBase() = default;

		const ListIteratorConst<GridRenderData> GetRenderIterator()const override final;

	protected:
		math::Vec3 CalculateGridPos(const EditorEntity*)const;
		math::Vec3 CalculateGridPosLocked(const EditorEntity*, float scale, bool ignoreSnapY)const;
		void CalculateGridPosAngleAxisY(const EditorEntity*, const math::CameraTransform&, GridRenderData&);

		List<GridRenderData> m_renderData;
		Handle<Entity> m_hEntity;
};

//=============================================================================
//	GridLockAction_EditorEntitySelect
//=============================================================================

class GridLockAction_EditorEntitySelect : public GridLockActionBase
{
	public:
		GridLockAction_EditorEntitySelect();
		virtual ~GridLockAction_EditorEntitySelect() = default;

		void Lock(const GridLockActionInfo&)override;
		void Update(const math::CameraTransform&, float scale)override;
};

//=============================================================================
//	GridLockAction_EditorToolTranslate
//=============================================================================

class GridLockAction_EditorToolTranslate : public GridLockActionBase
{
	public:
		GridLockAction_EditorToolTranslate();
		virtual ~GridLockAction_EditorToolTranslate() = default;

		void Lock(const GridLockActionInfo& info)override;
		void Update(const math::CameraTransform&, float scale)override;

	private:
		math::eAxis m_axisEnum;
};

//=============================================================================
//	GridLockAction_EditorToolResize
//=============================================================================

class GridLockAction_EditorToolResize : public GridLockActionBase
{
	public:
		GridLockAction_EditorToolResize();
		virtual ~GridLockAction_EditorToolResize() = default;

		void Lock(const GridLockActionInfo&)override;
		void Update(const math::CameraTransform&, float scale)override;

	private:
		math::eAxis m_axisEnum;
};

//=============================================================================
//	GridLockAction_HideGrid
//=============================================================================

class GridLockAction_HideGrid : public GridLockActionBase
{
	public:
	GridLockAction_HideGrid();
		virtual ~GridLockAction_HideGrid();

		void Lock(const GridLockActionInfo&)override;
		void Update(const math::CameraTransform&, float scale)override {}

	private:
		bool m_wasGridShowing;
};
