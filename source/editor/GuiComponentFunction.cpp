#include "GuiComponentFunction.h"
#include "GuiComponent.h"
#include "../engine/CoreInterface.h"
#include "../engine/Event.h"
#include "EditorEvent.h"
#include "../engine/Pointer.h"
#include "EditorEntityUtil.h"
#include "../shared/Command.h"

//-------------------------------------------------------------------
//	button_createEntity
//-------------------------------------------------------------------
void button_createEntity(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = (eEditorEntityType)flag;
	pEvent->action = eGuiAction::CreateEntity;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_pickAxis
//-------------------------------------------------------------------
void button_pickAxis(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = (eEditorEntityType)flag;
	pEvent->action = eGuiAction::PickAxis;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_pickEntity
//-------------------------------------------------------------------
void button_pickEntity(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = (eEditorEntityType)flag;
	pEvent->action = eGuiAction::PickEntity;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_cameraTrackDisconnect
//-------------------------------------------------------------------
void button_cameraTrackDisconnect(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = (eEditorEntityType)flag;
	pEvent->action = eGuiAction::CameraTrackDisconnect;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_openTextureSelectMenu
//-------------------------------------------------------------------
void button_openTextureSelectMenu(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = eEditorEntityType::Unknown;
	pEvent->action = eGuiAction::OpenTextureSelectMenu;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_openModelSelectMenu
//-------------------------------------------------------------------
void button_openModelSelectMenu(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = eEditorEntityType::Unknown;
	pEvent->action = eGuiAction::OpenModelSelectMenu;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_openEntitySelectMenu
//-------------------------------------------------------------------
void button_openEntitySelectMenu(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = eEditorEntityType::Unknown;
	pEvent->action = eGuiAction::OpenEntitySelectMenu;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_openTriggerSelectMenu
//-------------------------------------------------------------------
void button_openTriggerSelectMenu(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = eEditorEntityType::Unknown;
	pEvent->action = eGuiAction::OpenTriggerSelectMenu;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_openTextArea
//-------------------------------------------------------------------
void button_openTextArea(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
	pEvent->entityType = eEditorEntityType::Unknown;
	pEvent->action = eGuiAction::OpenTextArea;
	pEvent->flag = flag;

	g_pEvent->Process( pEvent );
}

//-------------------------------------------------------------------
//	button_selectDiffuseMap
//-------------------------------------------------------------------
void button_selectDiffuseMap(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ChangeTexture> pEvent = new Event_ChangeTexture;
	pEvent->textureType = eGuiTexture::DiffuseMap;
	pEvent->textureId = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_selectSpecularMap
//-------------------------------------------------------------------
void button_selectSpecularMap(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ChangeTexture> pEvent = new Event_ChangeTexture;
	pEvent->textureType = eGuiTexture::SpecularMap;
	pEvent->textureId = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_selectNormalMap
//-------------------------------------------------------------------
void button_selectNormalMap(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ChangeTexture> pEvent = new Event_ChangeTexture;
	pEvent->textureType = eGuiTexture::NormalMap;
	pEvent->textureId = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	button_selectModel
//-------------------------------------------------------------------
void button_selectModel(IGuiComponent*, uint flag)
{
	SmartPtr<Event_ChangeModel> pEvent = new Event_ChangeModel;
	pEvent->modelId = flag;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	gui_entityProcessCommand
//-------------------------------------------------------------------
void gui_entityProcessCommand_Float(IGuiComponent* pComponent, float value)
{
	SmartPtr<Event_GuiUse> pEvent = new Event_GuiUse;
	pEvent->action = eGuiAction::EntityProcessCommand;
	pEvent->command = command::formatFloat( pComponent->GetCommand(), value );

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
void gui_entityProcessCommand_String(IGuiComponent* pComponent, const String& s)
{
	SmartPtr<Event_GuiUse> pEvent = new Event_GuiUse;
	pEvent->action = eGuiAction::EntityProcessCommand;
	pEvent->command = pComponent->GetCommand() + " " + string::quote(s);

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
void gui_entityProcessCommand_Bool(IGuiComponent* pComponent, bool b)
{
	SmartPtr<Event_GuiUse> pEvent = new Event_GuiUse;
	pEvent->action = eGuiAction::EntityProcessCommand;
	pEvent->command = pComponent->GetCommand() + " " + string::boolToStr(b);

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	gui_entityProcessCommand_StringChain
//-------------------------------------------------------------------
void gui_entityProcessCommand_StringChain(IGuiComponent* pComponent, const List<String>& cmds, uint flag)
{
	uint chainCount = 0;

	if ( cmds.Count() == 0 )
	{
		SmartPtr<Event_GuiUse> pEvent = new Event_GuiUse;
		pEvent->action = eGuiAction::EntityProcessCommandChain;
		pEvent->chainCurrent = ++chainCount;

		g_pEvent->Process( pEvent );
	}
	else
	{
		for ( auto it = cmds.GetIterator(); it; it++ )
		{
			SmartPtr<Event_GuiUse> pEvent = new Event_GuiUse;
			pEvent->action = eGuiAction::EntityProcessCommandChain;
			pEvent->chainCurrent = ++chainCount;
			pEvent->chainCount = cmds.Count();
			pEvent->command = *it;
			pEvent->flag = flag;

			g_pEvent->Process( pEvent );
		}
	}
}
