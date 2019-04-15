#include "EditorEntity_TriggerCamera.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "../engine/Debug.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/ScriptReader.h"
#include "../shared/EntityUtil.h"
#include "../engine/Path.h"
#include "EditorEntity_CameraTrack.h"
#include "EditorEntity_CameraTrackPoint.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	EditorEntity_TriggerCamera
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_TriggerCamera::EditorEntity_TriggerCamera() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::OnShutdown_EditorEntity()
{
	g_pScene->Debug_Remove( m_hLine );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->color4 = math::normalizeRgb( 255, 70 );
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 0.0f;
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorTriggerCamera.tga"), false );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );

	m_hLine = g_pScene->Debug_AddLine();
	m_hLine->visible = false;
	m_hLine->color4 = math::normalizeRgb( 102, 216, 255 );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::Update()
{
	if ( m_hCameraTrack )
	{
		uint trackId = m_hCameraTrack.GetAs<EditorEntity_CameraTrack>().GetTrackId();
		math::Vec3 point0 = GetPos();
		math::Vec3 point1 = m_hCameraTrack.GetAs<EditorEntity>().GetPos();
		m_hLine->point0 = point0;
		m_hLine->point1 = point1;
	}
	else
		m_hLine->visible = false;
}
//-------------------------------------------------------------------
//	OnPickEntity
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::OnPickEntity(Handle<Entity> hEntity)
{
	if ( !hEntity->GetClassRelationship_Is<EditorEntity_CameraTrackPoint>() && !hEntity->GetClassRelationship_Is<EditorEntity_CameraTrack>() )
		return;

	CHECK( !m_pProcessTrackFind || m_pProcessTrackFind->GetState() != eProcessState::Running );

	ConnectToTrack( hEntity );
}
//-------------------------------------------------------------------
//	ConnectToTrack
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::ConnectToTrack(Handle<Entity> hEntity)
{
	EditorEntity_CameraTrack* pCameraTrack = nullptr;

	if ( hEntity->GetClassRelationship_Is<EditorEntity_CameraTrackPoint>() )
		pCameraTrack = hEntity.GetAs<EditorEntity_CameraTrackPoint>().GetOwnerPtr();
	else if ( hEntity->GetClassRelationship_Is<EditorEntity_CameraTrack>() )
		pCameraTrack = (EditorEntity_CameraTrack*)hEntity.GetPtr();

	if ( pCameraTrack )
	{
		m_hCameraTrack = pCameraTrack->GetHandle();
		uint trackId = m_hCameraTrack.GetAs<EditorEntity_CameraTrack>().GetTrackId();
		m_hLine->visible = true;
	}
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_TriggerCamera::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_TRIGGER_CAMERA) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeLine(command::TRACK_ID, string::intToStr(m_hCameraTrack ? m_hCameraTrack.GetAs<EditorEntity_CameraTrack>().GetTrackId() : 0) ) + 
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_TriggerCamera::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;
	uint trackIdConnected = 0;

	isCommandValid |= pLine->HasString( command::ENTITY_TRIGGER_CAMERA );
	isCommandValid |= pLine->SetInt( command::TRACK_ID, (int&)trackIdConnected );
	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( trackIdConnected > 0 )
	{
		m_pProcessTrackFind = new Process_EditorCameraTrackFind();
		m_pProcessTrackFind->SetOwner( GetHandle() );
		m_pProcessTrackFind->SetTrackIdToFind( trackIdConnected );
		g_pProcess->Add( m_pProcessTrackFind.As<Process>() );
	}

	CHECK( isCommandValid );
}

/****************************************************************************************
	Process_EditorTrackFind
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_EditorCameraTrackFind::Process_EditorCameraTrackFind()
{
	m_trackId = 0;
}
//-------------------------------------------------------------------
//	SetTrackIdToFind
//-------------------------------------------------------------------
void Process_EditorCameraTrackFind::SetTrackIdToFind(uint trackId)
{
	m_trackId = trackId;
}
//-------------------------------------------------------------------
//	SetOwner
//-------------------------------------------------------------------
void Process_EditorCameraTrackFind::SetOwner(Handle<Entity> hOwner)
{
	if ( hOwner->GetClassRelationship_Is<EditorEntity_TriggerCamera>() )
		m_hOwner = hOwner;
	else
	{
		m_hOwner.Invalidate();
		ASSERT( "Invalid owner type" );
	}
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void Process_EditorCameraTrackFind::OnEnter()
{
	CHECK( m_hOwner );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_EditorCameraTrackFind::Update()
{
	CHECK( m_trackId > 0 );
	Handle<Entity> hTrack = g_pEditor->GetEntityFinder()->Find( (uint)eEditorEntityType::CameraTrack, m_trackId );

	if ( hTrack )
	{
		m_hOwner.GetAs<EditorEntity_TriggerCamera>().ConnectToTrack( hTrack );
		Exit();
	}
}
