#include "EditorEvent.h"
#include "../engine/CoreInterface.h"
#include "EditorGlobal.h"
#include "EditorEntity.h"
#include "../engine/Pointer.h"

//-------------------------------------------------------------------
//	sendEvent_EntitySelect
//-------------------------------------------------------------------
void sendEvent_EntitySelect(Handle<Entity> hEntity)
{
	CHECK( hEntity->GetClassRelationship<EditorEntity>() == eClassRelationship::Inherits );

	SmartPtr<Event_EntitySelect> pEvent = new Event_EntitySelect;
	pEvent->hEntity = hEntity;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	sendEvent_EntityUnselect
//-------------------------------------------------------------------
void sendEvent_EntityUnselect()
{
	SmartPtr<Event_EntityUnselect> pEvent = new Event_EntityUnselect;

	g_pEvent->Process( pEvent );
}