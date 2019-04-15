#include "EditorEntity.h"
#include "../engine/Vertex.h"
#include "../engine/Model.h"
#include "EditorGlobal.h"
#include "RenderInfoEditor.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity::EditorEntity(bool mustUpdate) : Entity(mustUpdate)
{
	m_toolActionFlag = TOOL_ALLOW_NONE;
	m_adjustPosToPair = true;
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
EditorEntity::~EditorEntity()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void EditorEntity::OnInitialize()
{
	m_hModel = primitive::geometry::createBlock();
	OnInitialize_EditorEntity();

	auto pCmpRender = GetComponentPtrAs<Component_Render>();

	if ( pCmpRender )
		pCmpRender->UpdateTransform();
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void EditorEntity::OnShutdown()
{
	OnShutdown_EditorEntity();
}
//-------------------------------------------------------------------
//	SetGridPos
//-------------------------------------------------------------------
void EditorEntity::SetGridPos(int x, int y, int z, bool allowAdjust)
{
	SetGridPos( VecGrid(x, y, z), allowAdjust );
}
//-------------------------------------------------------------------
void EditorEntity::SetGridPos(const VecGrid& v, bool allowAdjust)
{
	VecGrid oldPos = m_gridPos;
	m_gridPos = v;

	if ( allowAdjust )
		AlignGridPosToPair();

	if ( oldPos != m_gridPos )
		AdjustTransform();
}
//-------------------------------------------------------------------
//	AddGridPos
//-------------------------------------------------------------------
void EditorEntity::AddGridPos(int x, int y, int z)
{
	AddGridPos( VecGrid(x, y, z) );
}
//-------------------------------------------------------------------
void EditorEntity::AddGridPos(const VecGrid& v)
{
	m_gridPos += v;
	AlignGridPosToPair();
	AdjustTransform();
}
//-------------------------------------------------------------------
//	SetGridScale
//-------------------------------------------------------------------
void EditorEntity::SetGridScale(int x, int y, int z)
{
	SetGridScale( VecGrid(x, y, z) );
}
//-------------------------------------------------------------------
void EditorEntity::SetGridScale(const VecGrid& v)
{
	m_gridScale = v;
	AdjustTransform();
}
//-------------------------------------------------------------------
//	AddGridScale
//-------------------------------------------------------------------
void EditorEntity::AddGridScale(int value, math::eAxis axis, math::eSign sign)
{
	VecGrid oldScale = m_gridScale;
	m_gridScale += AXES_GRID[(uint)axis] * value;

	m_gridScale.x = math::max( m_gridScale.x, (int)editorGlobal::entity::GRID_SCALE_MIN );
	m_gridScale.y = math::max( m_gridScale.y, (int)editorGlobal::entity::GRID_SCALE_MIN );
	m_gridScale.z = math::max( m_gridScale.z, (int)editorGlobal::entity::GRID_SCALE_MIN );

	m_gridPos += (m_gridScale - oldScale) * sign;
	AdjustTransform();
}
//-------------------------------------------------------------------
//	AlignGridPosToPair
//-------------------------------------------------------------------
void EditorEntity::AlignGridPosToPair()
{
	if ( !math::isPair(m_gridPos.x) ) m_gridPos.x += 1;
	if ( !math::isPair(m_gridPos.y) ) m_gridPos.y += 1;
	if ( !math::isPair(m_gridPos.z) ) m_gridPos.z += 1;
}
//-------------------------------------------------------------------
//	AdjustTransform
//-------------------------------------------------------------------
void EditorEntity::AdjustTransform()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetData(true);
	pRenderInfo->SetTranslation( GetPos() );
	pRenderInfo->SetScale( GetScale() );
}
//-------------------------------------------------------------------
//	ProcessCommand_GridPosScale
//-------------------------------------------------------------------
bool EditorEntity::ProcessCommand_GridPosScale(const ScriptLine* pLine)
{
	VecGrid gridData;

	if ( pLine->SetIntArray( command::GRID_POS, &gridData[0], 3 ) )
		SetGridPos( gridData );
	else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
		SetGridScale( gridData );
	else
		return false;

	return true;
}
//-------------------------------------------------------------------
//	ProcessCommand_Name
//-------------------------------------------------------------------
bool EditorEntity::ProcessCommand_Name(const ScriptLine* pLine)
{
	if ( pLine->HasString(command::NAME) )
	{
		String strData;
		
		if ( pLine->SetQuotedString(command::NAME, strData) )
			SetName( strData );
		else
			SetName("");

		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec3 EditorEntity::GetPos()const
{
	return m_gridPos.AsVec3() * g_pEditor->GetGrid()->GetSizeLevel(0);
}
//-------------------------------------------------------------------
//	GetGridPos
//-------------------------------------------------------------------
VecGrid EditorEntity::GetGridPos()const
{
	return m_gridPos;
}
//-------------------------------------------------------------------
//	GetGridScale
//-------------------------------------------------------------------
VecGrid EditorEntity::GetGridScale()const
{
	return m_gridScale;
}
//-------------------------------------------------------------------
//	GetAabb
//-------------------------------------------------------------------
math::Aabb3 EditorEntity::GetAabb()const
{
	math::Vec3 scale = GetScale();
	return math::Aabb3(
		-scale,
		scale
	);
}
//-------------------------------------------------------------------
//	GetAabbGlobal
//-------------------------------------------------------------------
math::Aabb3 EditorEntity::GetAabbGlobal()const
{
	return GetAabb() + GetPos();
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec3 EditorEntity::GetScale()const
{
	return m_gridScale.AsVec3() * g_pEditor->GetGrid()->GetSizeLevel(0);
}
//-------------------------------------------------------------------
//	GetModel
//-------------------------------------------------------------------
Handle<Model> EditorEntity::GetModel()
{
	return m_hModel;
}
//-------------------------------------------------------------------
//	OnPickEntity
//-------------------------------------------------------------------
void EditorEntity::OnPickEntity(Handle<Entity>)
{
	ASSERT( "This entity does not support picking" );
}
