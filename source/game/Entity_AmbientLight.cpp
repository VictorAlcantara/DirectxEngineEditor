#include "Entity_AmbientLight.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "../shared/EntityUtil.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_AmbientLight::Entity_AmbientLight() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_AmbientLight::OnShutdown()
{
	g_pScene->GetLightMngr()->SetAmbientLight( 0, 0, 0 );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_AmbientLight::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;
	math::Vec4 ambientLightColor4 = g_pScene->GetLightMngr()->GetAmbientLight();

	isCommandValid |= pLine->HasString( command::ENTITY_AMBIENT_LIGHT );
	isCommandValid |= pLine->HasString( command::GRID_POS );
	isCommandValid |= pLine->HasString( command::GRID_SCALE );
	isCommandValid |= processCommand_MaterialColor( command::AMBIENT_COLOR, &ambientLightColor4[0], ambientLightColor4.GetComponentCount(), pLine );

	g_pScene->GetLightMngr()->SetAmbientLight( ambientLightColor4 );

	CHECK( isCommandValid );
}
