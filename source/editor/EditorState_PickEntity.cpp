#include "EditorState_PickEntity.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "EditorState_Idle.h"
#include "EditorToolManager.h"
#include "../shared/FirstPersonCamera.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorState_PickEntity::EditorState_PickEntity()
{
	m_filter = eEditorEntityType::Unknown;
	m_isShowingGrid = true;
}
//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void EditorState_PickEntity::Enter()
{
	g_pEditor->GetGui()->HideAll();
	g_pEditor->GetEditorToolManager()->Detach();
	m_isShowingGrid = g_pEditor->GetGrid()->IsGridVisible();
	g_pEditor->GetGrid()->ShowGrid( false );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorState_PickEntity::Update()
{
	bool changeState = false;

	if ( !g_pEditor->GetGui()->IsCursorOnTopAnyPage() && g_pInput->IsKeyPress(KEY_MOUSE_LEFT) )
	{
		math::CameraTransform camera = g_pEditor->GetCamera()->GetCameraTransform();
		math::Vec3 mouseToWorld = math::mousePosToWorld( camera );
		Handle<Entity> hEntityPicked = g_pEditor->GetEntityAtRayIntersection( camera.pos, mouseToWorld );
		EditorEntity* pEditorEntity = (EditorEntity*)hEntityPicked.GetPtrOrNull();

		CHECK( !pEditorEntity || pEditorEntity->GetClassRelationship<EditorEntity>() != eClassRelationship::None );

		if ( pEditorEntity && ( !UsesFilter() || pEditorEntity->GetType() == m_filter ) )
		{
			g_pEditor->GetEntitySelected().GetAs<EditorEntity>().OnPickEntity( hEntityPicked );
			changeState = true;
		}
	}
	else if ( g_pInput->IsKeyPress(KEY_ESCAPE) )
	{
		changeState = true;
	}

	if ( changeState )
	{
		g_pEditor->GetEditorToolManager()->Attach( g_pEditor->GetEntitySelected() );
		g_pEditor->GetGrid()->ShowGrid( m_isShowingGrid );
		m_pOwner->ChangeState( new EditorState_Idle );
	}
}
