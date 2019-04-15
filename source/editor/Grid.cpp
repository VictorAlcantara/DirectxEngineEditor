#include "../editor/Grid.h"
#include "../engine/Model.h"
#include "../engine/Shader.h"
#include "../engine/Vertex.h"
#include "../engine/CoreInterface.h"
#include "EditorGlobal.h"
#include "EditorEvent.h"
#include "GridLockAction.h"
#include"../engine/Debug.h"

//---------------------------------------------------------------------------------------
//	Cdtor
//---------------------------------------------------------------------------------------
Grid::Grid()
{
	m_initialized = false;
	m_show = false;
}
//---------------------------------------------------------------------------------------
Grid::~Grid()
{
	CHECK( !m_initialized );
}
//---------------------------------------------------------------------------------------
//	Shutdown
//---------------------------------------------------------------------------------------
void Grid::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_pShader->Shutdown();
	m_pShader = nullptr;

	m_pModel->Shutdown();
	m_pModel = nullptr;

	m_initialized = false;
}
//---------------------------------------------------------------------------------------
//	Initialize
//---------------------------------------------------------------------------------------
void Grid::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	uint lineCount = editorGlobal::grid::LINE_COUNT;
	uint vertexCount = (lineCount * 4 + 2) * 2;
	Vertex_P3* vertices = new Vertex_P3[vertexCount];
	float dist = (float)lineCount;
	int i = 0;

	for ( int x = -((int)lineCount); x <= (int)lineCount; x++ )
	{
		vertices[i++].pos = math::Vec3( (float)x, 0.0f, -dist );
		vertices[i++].pos = math::Vec3( (float)x, 0.0f,  dist );
	}

	for ( int z = -((int)lineCount); z <= (int)lineCount; z++ )
	{
		vertices[i++].pos = math::Vec3( -dist, 0.0f, (float)z );
		vertices[i++].pos = math::Vec3(  dist, 0.0f, (float)z );
	}

	m_pModel = new Model;
	m_pModel->Initialize( eTopology::Line, vertices, vertexCount );

	m_pShader = new Shader_OneColor;
	m_pShader->Initialize();

	safeArrayRelease( vertices );

	InitializeListener();

	m_level = editorGlobal::grid::LEVEL_SIZE_ONE;

	m_show = true;

	m_initialized = true;
}
//---------------------------------------------------------------------------------------
//	Update
//---------------------------------------------------------------------------------------
void Grid::Update(const math::CameraTransform& camera)
{
	if ( g_pKeyMap->CanPerform(editorGlobal::action::GRID_DECREASE) )
		m_level -= m_level <= editorGlobal::grid::LEVEL_MIN ? 0 : 1; 
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::GRID_INCREASE) )
		m_level = math::min( m_level + 1, editorGlobal::grid::LEVEL_MAX );

	if ( m_pGridLockAction )
	{
		m_pGridLockAction->Update( camera, GetSizeCurrentLevel() );
	}
}
//---------------------------------------------------------------------------------------
//	Render
//---------------------------------------------------------------------------------------
void Grid::Render(const math::CameraTransform& camera)
{
	if ( m_pGridLockAction && m_show )
	{
		g_pRenderer->SetDepthOperation( eDepthOperation::LessEqual );

		for ( auto it = m_pGridLockAction->GetRenderIterator(); it; it++ )
		{
			GridRenderData data = *it;

			if ( !data.visible )
				continue;

			math::Mtx44 translate = math::matrixTranslate( data.pos );
			math::Mtx44 rotate = math::matrixYawPitchRoll( data.angle );
			math::Mtx44 scale = math::matrixScale( GetSizeCurrentLevel(), 1.0f, GetSizeCurrentLevel() );

			m_pShader->SetWvp( scale * rotate * translate, camera.view, camera.proj );
			m_pShader->SetColor( data.color );
			m_pShader->SetDepthBias( editorGlobal::grid::RENDER_DEPTH_BIAS );
			g_pRenderer->Render( m_pModel.Get(), m_pShader.Get() );
		}
	}
}
//---------------------------------------------------------------------------------------
//	GetCurrentLevel
//---------------------------------------------------------------------------------------
uint Grid::GetCurrentLevel()const
{
	return m_level;
}
//---------------------------------------------------------------------------------------
//	GetSizeLevel
//---------------------------------------------------------------------------------------
float Grid::GetSizeLevel(uint level)
{
	return GetUnitLevel(level) * editorGlobal::grid::SIZE_UNIT_ONE;
}
//---------------------------------------------------------------------------------------
//	GetSizeCurrentLevel
//---------------------------------------------------------------------------------------
float Grid::GetSizeCurrentLevel()const
{
	return GetSizeLevel( m_level );
}
//---------------------------------------------------------------------------------------
//	GetUnitLevel
//---------------------------------------------------------------------------------------
uint Grid::GetUnitLevel(uint level)
{
	return (uint)math::pow( 2, math::min(level, editorGlobal::grid::LEVEL_MAX) );
}
//---------------------------------------------------------------------------------------
//	GetUnitCurrentLevel
//---------------------------------------------------------------------------------------
uint Grid::GetUnitCurrentLevel()const
{
	return GetUnitLevel( m_level );
}
//---------------------------------------------------------------------------------------
//	ToVecGrid
//---------------------------------------------------------------------------------------
VecGrid Grid::ToVecGrid(const math::Vec3& v)
{
	float unitsLevel0 = GetSizeLevel(0);
	return VecGrid( (int)(v.x / unitsLevel0), (int)(v.y / unitsLevel0), (int)(v.z / unitsLevel0) );
}
//---------------------------------------------------------------------------------------
//	ToVec3
//---------------------------------------------------------------------------------------
math::Vec3 Grid::ToVec3(const VecGrid& vg)
{
	float unitsLevel0 = GetSizeLevel(0);
	return math::Vec3((float)vg.x, (float)vg.y, (float)vg.z) * unitsLevel0;
}
//---------------------------------------------------------------------------------------
//	SnapToGrid
//---------------------------------------------------------------------------------------
int Grid::SnapToGrid(int value, math::eSign direction, uint units)const
{
	int valueOnGrid = value % (int)units;

	if ( valueOnGrid == 0 )
		return 0;

	if ( valueOnGrid < 0 )
		valueOnGrid += (int)units;

	int snapTo = direction == math::POSITIVE ? (int)units : 0;

	return snapTo - valueOnGrid;
}
//---------------------------------------------------------------------------------------
VecGrid Grid::SnapToGrid(const math::Vec3& value, uint units)const
{
	return VecGrid(
		(int)(value.x / (float)units) * (int)units,
		(int)(value.y / (float)units) * (int)units,
		(int)(value.z / (float)units) * (int)units
	);
}
//---------------------------------------------------------------------------------------
//	HandleEvent
//---------------------------------------------------------------------------------------
void Grid::HandleEvent(Event* pEvent)
{
	Event_EntityToolUse* pEventEntityToolUse = nullptr;
	Event_EntitySelect* pEventEntitySelect = nullptr;
	Event_EntityToolEndUse* pEventEntityToolEndUse = nullptr;

	if ( isEvent<Event_EntitySelect>(pEvent, &pEventEntitySelect) )
	{
		GridLockActionInfo info;
		info.hEntity = pEventEntitySelect->hEntity;
		info.isShowingGrid = m_show;

		m_pGridLockAction = new GridLockAction_EditorEntitySelect;
		m_pGridLockAction->Lock( info );
	}
	else if ( isEvent<Event_EntityUnselect>(pEvent) )
	{
		m_pGridLockAction = nullptr;
	}
	else if ( isEvent<Event_EntityToolUse>(pEvent, &pEventEntityToolUse) )
	{
		GridLockActionInfo info;
		info.hEntity = pEventEntityToolUse->hEntity;
		info.axisEnum = pEventEntityToolUse->axisEnum;
		info.axisSign = pEventEntityToolUse->axisSign;
		info.isShowingGrid = m_show;

		if ( pEventEntityToolUse->entityToolAction == eEditorToolAction::Translate )
			m_pGridLockAction = new GridLockAction_EditorToolTranslate;
		else if ( pEventEntityToolUse->entityToolAction == eEditorToolAction::Resize )
			m_pGridLockAction = new GridLockAction_EditorToolResize;
		else if ( pEventEntityToolUse->entityToolAction == eEditorToolAction::Rotate )
			m_pGridLockAction = new GridLockAction_HideGrid;
		else if ( pEventEntityToolUse->entityToolAction == eEditorToolAction::PickAxis )
			m_pGridLockAction = new GridLockAction_HideGrid;
		else
			ASSERT( "Unrecognized entity tool action" );

		m_pGridLockAction->Lock( info );
	}
	else if ( isEvent<Event_EntityToolEndUse>(pEvent, &pEventEntityToolEndUse) )
	{
		GridLockActionInfo info;
		info.hEntity = pEventEntityToolEndUse->hEntity;

		m_pGridLockAction = new GridLockAction_EditorEntitySelect;
		m_pGridLockAction->Lock( info );
	}
}
