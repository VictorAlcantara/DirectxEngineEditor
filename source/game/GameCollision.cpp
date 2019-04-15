//#include "GameCollision.h"
//#include "../engine/Entity.h"
//#include "Entity_Rocket.h"
//#include "Entity_Block.h"
//#include "Entity_Player.h"
//#include "Entity_Door.h"
//#include "Entity_TriggerCamera.h"
//#include "Entity_TriggerCommand.h"
//#include "Entity_MonsterProjectile.h"
//#include "Entity_Monster.h"
//#include "GameTrigger.h"
//#include "GameGlobal.h"
//#include "../engine/Pointer.h"
//#include "../engine/CoreInterface.h"
//#include "GameEvent.h"
//
////=============================================================================
////	Ignore
////=============================================================================
//
////-------------------------------------------------------------------
////	collisionIgnoreFunc_Projectile
////-------------------------------------------------------------------
//bool collisionIgnoreFunc_Projectile(Handle<Entity> entityMain, Handle<Entity> entityOther)
//{
//	CHECK( entityMain->GetClassRelationship_Is<Entity_MonsterProjectile>() );
//
//	return entityOther->GetClassRelationship_Is<Entity_Monster>() || entityOther->GetClassRelationship_Is<Entity_MonsterProjectile>();
//}
////-------------------------------------------------------------------
////	collisionIgnoreFunc_Character
////-------------------------------------------------------------------
//bool collisionIgnoreFunc_Character(Handle<Entity> entityMain, Handle<Entity> entityOther)
//{
//	return entityOther->GetClassRelationship<Entity_Player>() == eClassRelationship::None;
//}
//
////=============================================================================
////	Classify
////=============================================================================
//
////=============================================================================
////	Trigger
////=============================================================================
//
////-------------------------------------------------------------------
////	executeTriggerCommand
////-------------------------------------------------------------------
//void executeTriggerCommand(Handle<Entity>& hTrigger)
//{
//	if ( hTrigger->GetClassRelationship_Is<Entity_TriggerCommand>() )
//	{
//		for ( auto it = hTrigger.GetAs<Entity_TriggerCommand>().GetCommands().GetIterator(); it; it++ )
//			g_pCommand->Execute( *it );
//	}
//	else
//		ASSERT( "Invalid trigger type" );
//}
////-------------------------------------------------------------------
////	collisionTriggerEnterFunc_TriggerCamera
////-------------------------------------------------------------------
//void collisionTriggerEnterFunc_TriggerCamera(Handle<Entity> hTrigger, Handle<Entity> hEntity)
//{
//	if ( hEntity->GetClassRelationship_Is<Entity_Player>() && hTrigger->GetClassRelationship_Is<Entity_TriggerCamera>() )
//	{
//		SmartPtr<Event_TriggerCamera> pEvent = new Event_TriggerCamera;
//		pEvent->hTrigger = hTrigger;
//		pEvent->hEntity = hEntity;
//
//		g_pEvent->Process( pEvent );
//	}
//}
////-------------------------------------------------------------------
////	collisionTriggerEnterFunc_Command
////-------------------------------------------------------------------
//void collisionTriggerEnterFunc_Command(Handle<Entity> hTrigger, Handle<Entity> hEntity)
//{
//	executeTriggerCommand( hTrigger );
//}
////-------------------------------------------------------------------
////	collisionTriggerUpdateFunc_Command
////-------------------------------------------------------------------
//void collisionTriggerUpdateFunc_Command(Handle<Entity> hTrigger, Handle<Entity> hEntity)
//{
//	executeTriggerCommand( hTrigger );	
//}
////-------------------------------------------------------------------
////	collisionTriggerExitFunc_Command
////-------------------------------------------------------------------
//void collisionTriggerExitFunc_Command(Handle<Entity> hTrigger)
//{
//	executeTriggerCommand( hTrigger );
//}
