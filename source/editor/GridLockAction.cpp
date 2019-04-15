#include "GridLockAction.h"
#include "EditorGlobal.h"
#include "EditorEntity.h"

//=============================================================================
//	GridLockActionBase
//=============================================================================

//-------------------------------------------------------------------
//	GetRenderIterator
//-------------------------------------------------------------------
const ListIteratorConst<GridRenderData> GridLockActionBase::GetRenderIterator()const
{
	return m_renderData.GetIterator();
}
//-------------------------------------------------------------------
//	CalculateGridPos
//-------------------------------------------------------------------
math::Vec3 GridLockActionBase::CalculateGridPos(const EditorEntity* pEntity)const
{
	math::Vec3 result = pEntity->GetPos();

	result.y += -pEntity->GetScale().y;

	return result;
}
//-------------------------------------------------------------------
//	CalculateGridPosLocked
//-------------------------------------------------------------------
math::Vec3 GridLockActionBase::CalculateGridPosLocked(const EditorEntity* pEntity, float scale, bool ignoreSnapY)const
{
	math::Vec3 result = pEntity->GetPos();

	result.x = (int)(result.x / scale) * scale;
	if ( ignoreSnapY )
		result.y += -pEntity->GetScale().y;
	else
		result.y = (int)(result.y / scale) * scale;
	result.z = (int)(result.z / scale) * scale;

	return result;
}
//-------------------------------------------------------------------
//	CalculateGridPosAngleAxisY
//-------------------------------------------------------------------
void GridLockActionBase::CalculateGridPosAngleAxisY(const EditorEntity* pEntity, const math::CameraTransform& camera, GridRenderData& renderData)
{
	math::Vec3 entityScale = pEntity->GetScale();
	math::Vec3 entityToCam = camera.pos - pEntity->GetPos();
	math::eAxis axisEnumToUse = math::eAxis::X;
	math::Vec3 axisToUse = math::AXIS_X;
	float dotToUse = entityToCam * math::AXIS_X;
	float dotZ = entityToCam * math::AXIS_Z;

	if ( math::abs(dotZ) > math::abs(dotToUse) )
	{
		dotToUse = dotZ;
		axisToUse = math::AXIS_Z;
		axisEnumToUse = math::eAxis::Z;
		renderData.angle.yaw = 0.0f;
	}
	else
		renderData.angle.yaw = 90.0f;

	renderData.angle.pitch = 90.0f;

	if ( math::abs(entityToCam * axisToUse) > math::abs(entityScale * axisToUse) )
	{
		float reposition = pEntity->GetPos() * axisToUse + entityScale * axisToUse * math::sign( dotToUse );
		if ( axisEnumToUse == math::eAxis::X )
			renderData.pos.x = reposition;
		else
			renderData.pos.z = reposition;
	}
}

//=============================================================================
//	GridLockAction_EditorEntitySelect
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GridLockAction_EditorEntitySelect::GridLockAction_EditorEntitySelect()
{
	GridRenderData data;
	data.color = editorGlobal::grid::COLOR_MAIN;
	m_renderData.AddLast( data );
}
//-------------------------------------------------------------------
//	Lock
//-------------------------------------------------------------------
void GridLockAction_EditorEntitySelect::Lock(const GridLockActionInfo& info)
{
	m_hEntity = info.hEntity;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GridLockAction_EditorEntitySelect::Update(const math::CameraTransform&, float scale)
{
	if ( !m_hEntity.IsValid() )
		return;

	GridRenderData* pData = &m_renderData.GetFirst()->GetValue();

	pData->pos = CalculateGridPosLocked( m_hEntity->Convert<EditorEntity>(), scale, true );
}

//=============================================================================
//	GridLockAction_EditorToolTranslate
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GridLockAction_EditorToolTranslate::GridLockAction_EditorToolTranslate()
{
	GridRenderData data;
	data.color = editorGlobal::grid::COLOR_GUIDE;
	m_renderData.AddLast( data );
}
//-------------------------------------------------------------------
//	Lock
//-------------------------------------------------------------------
void GridLockAction_EditorToolTranslate::Lock(const GridLockActionInfo& info)
{
	m_hEntity = info.hEntity;
	m_axisEnum = info.axisEnum;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GridLockAction_EditorToolTranslate::Update(const math::CameraTransform& camera, float scale)
{
	const EditorEntity* pEntity = m_hEntity->Convert<EditorEntity>();

	if ( !pEntity )
		return;

	GridRenderData* pData = &m_renderData.GetFirst()->GetValue();

	if ( m_axisEnum == math::eAxis::Y )
	{
		pData->pos = pEntity->GetPos();
		CalculateGridPosAngleAxisY( pEntity, camera, *pData );
	}
	else
	{
		pData->pos = CalculateGridPos( pEntity );
		pData->angle.pitch = 0.0f;
	}
}

//=============================================================================
//	GridLockAction_EditorToolResize
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GridLockAction_EditorToolResize::GridLockAction_EditorToolResize()
{
	GridRenderData data;
	data.color = editorGlobal::grid::COLOR_MAIN;
	data.angle.type = math::eAngle::Degree;
	m_renderData.AddLast( data );
}
//-------------------------------------------------------------------
//	Lock
//-------------------------------------------------------------------
void GridLockAction_EditorToolResize::Lock(const GridLockActionInfo& info)
{
	m_hEntity = info.hEntity;
	m_axisEnum = info.axisEnum;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GridLockAction_EditorToolResize::Update(const math::CameraTransform& camera, float scale)
{
	const EditorEntity* pEntity = (const EditorEntity*)m_hEntity->Convert<EditorEntity>();

	if ( !pEntity )
		return;

	GridRenderData* pData = &m_renderData.GetFirst()->GetValue();

	if ( m_axisEnum == math::eAxis::Y )
	{
		pData->pos = CalculateGridPosLocked( pEntity, scale, false );
		CalculateGridPosAngleAxisY( pEntity, camera, *pData );
	}
	else
	{
		pData->pos = CalculateGridPosLocked( pEntity, scale, true );
		pData->angle.pitch = 0.0f;
		pData->angle.yaw = 0.0f;
	}
}

//=============================================================================
//	GridLockAction_HideGrid
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GridLockAction_HideGrid::GridLockAction_HideGrid()
{
	m_wasGridShowing = true;
}
//-------------------------------------------------------------------
GridLockAction_HideGrid::~GridLockAction_HideGrid()
{
	g_pEditor->GetGrid()->ShowGrid( m_wasGridShowing );
}
//-------------------------------------------------------------------
//	Lock
//-------------------------------------------------------------------
void GridLockAction_HideGrid::Lock(const GridLockActionInfo& info)
{
	m_wasGridShowing = info.isShowingGrid;
	g_pEditor->GetGrid()->ShowGrid( false );
}
