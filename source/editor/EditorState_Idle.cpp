#include "EditorState_Idle.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "../engine/Math.h"
#include "EditorEvent.h"
#include "EditorGlobal.h"
#include "../shared/FirstPersonCamera.h"
#include "EditorToolManager.h"
#include "EditorState_BlockToolUse.h"
#include "GuiPage.h"
#include "GuiPageControl.h"

//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void EditorState_Idle::Enter()
{
	GuiPage* pPage = g_pEditor->GetGui()->GetStackTop();

	if ( pPage )
		pPage->Show( true );
	else
	{
		pPage = g_pEditor->GetGui()->FindPage( editorGlobal::guiPage::PAGE_MAIN_MENU );
		pPage->Show( true );
	}
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorState_Idle::Update()
{
	if ( g_pEditor->GetEditorToolManager()->IsUsing() )
		m_pOwner->ChangeState( new EditorState_EntityToolUse );
	else if ( !g_pEditor->GetGui()->IsCursorOnTopAnyPage() && g_pKeyMap->CanPerform(editorGlobal::action::ENTITY_SELECT) )
	{
		math::CameraTransform camera = g_pEditor->GetCamera()->GetCameraTransform();
		math::Vec3 mouseToWorld = math::mousePosToWorld( camera );
		Handle<Entity> hEntitySelected = g_pEditor->GetEntityAtRayIntersection( camera.pos, mouseToWorld );

		if ( hEntitySelected )
		{
			SelectEntity( hEntitySelected );
		}
		else
		{
			UnselectEntity();
		}
	}
	else if (  g_pKeyMap->CanPerform(editorGlobal::action::ESCAPE) )
	{
		if ( g_pEditor->GetGui()->GetStackTop()->GetName() == editorGlobal::guiPage::PAGE_TEXTURE_SELECT || 
			g_pEditor->GetGui()->GetStackTop()->GetName() == editorGlobal::guiPage::PAGE_MODEL_SELECT )
		{
			g_pEditor->GetGui()->GetStackTop()->Show( false );
			g_pEditor->GetGui()->RemoveStackTop();
			g_pEditor->GetGui()->GetStackTop()->Show( true );
		}
		else if ( g_pEditor->GetEntitySelected() )
			UnselectEntity();
		else
			g_pEditor->GetGuiPageControl()->ShowPageMain();
	}
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::ENTITY_DELETE) )
	{
		Handle<Entity> hEntitySelected = g_pEditor->GetEntitySelected();
		if ( hEntitySelected && hEntitySelected.GetAs<EditorEntity>().CanDelete() )
		{
			UnselectEntity();
			g_pEntity->Remove( hEntitySelected );
			hEntitySelected.Invalidate();
		}
	}
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::ENTITY_CLONE) )
	{
		Handle<Entity> hEntitySelected = g_pEditor->GetEntitySelected();

		if ( hEntitySelected && hEntitySelected.GetAs<EditorEntity>().CanClone() )
		{
			Handle<Entity> hEntityClone = g_pEditor->EntityClone( hEntitySelected->Convert<EditorEntity>() );
			
			SelectEntity( hEntityClone );
		}
	}
}
//-------------------------------------------------------------------
//	SelectEntity
//-------------------------------------------------------------------
void EditorState_Idle::SelectEntity(Handle<Entity> hEntity)
{
	sendEvent_EntitySelect( hEntity );

	if ( !g_pEditor->GetGuiPageControl()->ShowPageProperty( hEntity->Convert<EditorEntity>() ) )
		g_pEditor->GetGuiPageControl()->ShowPageMain();
}
//-------------------------------------------------------------------
//	UnselectEntity
//-------------------------------------------------------------------
void EditorState_Idle::UnselectEntity()
{
	sendEvent_EntityUnselect();
	g_pEditor->GetGuiPageControl()->ShowPageMain();
}
