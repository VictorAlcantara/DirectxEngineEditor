#include "EditorState_PickAxis.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "EditorToolManager.h"
#include "EditorEntity_Door.h"
#include "EditorState_Idle.h"
#include "EditorEvent.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorState_PickAxis::EditorState_PickAxis()
{
}
//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void EditorState_PickAxis::Enter()
{
	g_pEditor->GetGui()->HideAll();
	InitializeListener();
}
//-------------------------------------------------------------------
//	Exit
//-------------------------------------------------------------------
void EditorState_PickAxis::Exit()
{
	ShutdownListener();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorState_PickAxis::Update()
{
	bool changeState = false;

	if ( !math::isZero(m_openDirection) )
	{
		Handle<Entity> hEntity = g_pEditor->GetEntitySelected();

		if ( hEntity->GetClassRelationship_Is<EditorEntity_Door>() )
		{
			EditorEntity_Door* pDoor = (EditorEntity_Door*)hEntity.GetPtr();
			pDoor->SetOpenDirection( m_openDirection );
			m_pOwner->ChangeState( new EditorState_Idle );
		}
	}

	if ( g_pInput->IsKeyPress(KEY_ESCAPE) )
		changeState = true;

	if ( changeState )
		m_pOwner->ChangeState( new EditorState_Idle );
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void EditorState_PickAxis::HandleEvent(Event* pEvent)
{
	Event_EditorToolUse* pEventTool = nullptr;

	if ( isEvent<Event_EditorToolUse>(pEvent, &pEventTool) )
	{
		m_openDirection = math::AXES[pEventTool->axisEnum] * (float)pEventTool->axisSign;
	}
}
