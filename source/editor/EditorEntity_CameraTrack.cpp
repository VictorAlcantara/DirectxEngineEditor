#include "EditorEntity_CameraTrack.h"
#include "../engine/CoreInterface.h"
#include "RenderInfoEditor.h"
#include "../engine/Primitive.h"
#include "EditorGlobal.h"
#include "../shared/Command.h"
#include "EditorEntity_CameraTrackPoint.h"
#include "../engine/Component_Render.h"

uint EditorEntity_CameraTrack::s_trackIdControl = 0;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_CameraTrack::EditorEntity_CameraTrack() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );

	m_isClone = false;

	do 
	{
		s_trackIdControl++;
	} while ( g_pEditor && g_pEditor->GetEntityFinder() && g_pEditor->GetEntityFinder()->Find((uint)GetType(), s_trackIdControl) );

	m_trackId = s_trackIdControl;
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::OnShutdown_EditorEntity()
{
	g_pEntity->Remove( m_hTrackPoint0 );
	g_pEntity->Remove( m_hTrackPoint1 );
	g_pScene->Debug_Remove( m_hLine0 );
	g_pScene->Debug_Remove( m_hLine1 );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->color4 = math::saturateRgb( math::normalizeRgb(255, 200, 100), 0.5f );
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 0.0f;
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.2f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );

	int pointDistance = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 5.0f);
	m_hTrackPoint0 = g_pEditor->EntityCreate( eEditorEntityType::CameraTrackPoint );
	m_hTrackPoint0.GetAs<EditorEntity_CameraTrackPoint>().SetOwner( GetHandle() );
	m_hTrackPoint0.GetAs<EditorEntity_CameraTrackPoint>().SetGridPos( GetGridPos() + VecGrid( -pointDistance, 0, 0 ) );

	m_hTrackPoint1 = g_pEditor->EntityCreate( eEditorEntityType::CameraTrackPoint );
	m_hTrackPoint1.GetAs<EditorEntity_CameraTrackPoint>().SetOwner( GetHandle() );
	m_hTrackPoint1.GetAs<EditorEntity_CameraTrackPoint>().SetGridPos( GetGridPos() + VecGrid( pointDistance, 0, 0 ) );

	m_hLine0 = g_pScene->Debug_AddLine();
	m_hLine0->color4 = math::normalizeRgb( 255, 200, 0 );
	m_hLine1 = g_pScene->Debug_AddLine();
	m_hLine1->color4 = m_hLine0->color4;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::Update()
{
	AdjustPointPosition();
}
//-------------------------------------------------------------------
//	OnToolUse
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::OnToolUse()
{
	AdjustPointPosition();
}
//-------------------------------------------------------------------
//	AdjustPointPosition
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::AdjustPointPosition()
{
	auto pTrack0 = m_hTrackPoint0->Convert<EditorEntity_CameraTrackPoint>();
	auto pTrack1 = m_hTrackPoint1->Convert<EditorEntity_CameraTrackPoint>();

	math::Vec3 trackLine = pTrack1->GetPos() - pTrack0->GetPos();

	math::Vec3 newPos0 = (GetPos() - trackLine * 0.5f) * (float)editorGlobal::grid::UNIT_SIZE_ONE;
	math::Vec3 newPos1 = (GetPos() + trackLine * 0.5f) * (float)editorGlobal::grid::UNIT_SIZE_ONE;

	pTrack0->SetGridPos( newPos0 );
	pTrack1->SetGridPos( newPos1 );

	m_hLine0->point0 = GetPos();
	m_hLine0->point1 = pTrack0->GetPos();

	m_hLine1->point0 = GetPos();
	m_hLine1->point1 = pTrack1->GetPos();
}
//-------------------------------------------------------------------
//	OnClone
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::OnClone()
{
	m_isClone = true;
}
//-------------------------------------------------------------------
//	AssignNewTrackId
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::AssignNewTrackId(uint newId)
{
	g_pEditor->GetEntityFinder()->Remove( GetHandle() );
	m_trackId = newId;
	g_pEditor->GetEntityFinder()->Add( GetHandle() );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_CameraTrack::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_CAMERA_TRACK) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::TRACK_ID, string::intToStr(m_trackId) ) + 
		command::serializeLine(command::TRACK_POINT0_GRID_POS, m_hTrackPoint0.GetAs<EditorEntity>().GetGridPos().AsString() ) + 
		command::serializeLine(command::TRACK_POINT1_GRID_POS, m_hTrackPoint1.GetAs<EditorEntity>().GetGridPos().AsString() ) + 
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_CameraTrack::ProcessCommand(const ScriptLine* pLine)
{
	uint trackId = 0;
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_CAMERA_TRACK );
	isCommandValid |= pLine->SetInt( command::TRACK_ID, (int&)trackId );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( !isCommandValid )
	{
		VecGrid pointPos;

		isCommandValid = true;

		if ( pLine->SetIntArray( command::TRACK_POINT0_GRID_POS, &pointPos[0], 3 ) )
			m_hTrackPoint0.GetAs<EditorEntity>().SetGridPos( pointPos );
		else if ( pLine->SetIntArray( command::TRACK_POINT1_GRID_POS, &pointPos[0], 3 ) )
			m_hTrackPoint1.GetAs<EditorEntity>().SetGridPos( pointPos );
		else
			isCommandValid = false;
	}

	if ( !m_isClone && trackId > 0 )
		AssignNewTrackId( trackId );

	CHECK( isCommandValid );
}
//-------------------------------------------------------------------
//	GetTrackPoint
//-------------------------------------------------------------------
Handle<Entity> EditorEntity_CameraTrack::GetTrackPoint0()
{
	return m_hTrackPoint0;
}
//-------------------------------------------------------------------
Handle<Entity> EditorEntity_CameraTrack::GetTrackPoint1()
{
	return m_hTrackPoint1;
}
