#include "EditorState_BlockToolUse.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "EditorState_Idle.h"
#include "EditorToolManager.h"

//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void EditorState_EntityToolUse::Enter()
{
	g_pEditor->GetGui()->HideAll();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorState_EntityToolUse::Update()
{
	if ( !g_pEditor->GetEditorToolManager()->IsUsing() )
	{
		if ( g_pInput->IsKeyDown( KEY_ESCAPE ) )
			g_pEditor->GetEditorToolManager()->Attach( Handle<Entity>() );
		else
			m_pOwner->ChangeState( new EditorState_Idle );
	}
}
