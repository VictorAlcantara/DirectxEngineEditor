#pragma once

#include "GuiUtil.h"

void button_createEntity(IGuiComponent*, uint flag);
void button_pickAxis(IGuiComponent*, uint flag);
void button_pickEntity(IGuiComponent*, uint flag);
void button_cameraTrackDisconnect(IGuiComponent*, uint flag);
void button_openTextureSelectMenu(IGuiComponent*, uint flag);
void button_openModelSelectMenu(IGuiComponent*, uint flag);
void button_openEntitySelectMenu(IGuiComponent*, uint flag);
void button_openTriggerSelectMenu(IGuiComponent*, uint flag);
void button_openTextArea(IGuiComponent*, uint flag);
void button_selectDiffuseMap(IGuiComponent*, uint flag);
void button_selectSpecularMap(IGuiComponent*, uint flag);
void button_selectNormalMap(IGuiComponent*, uint flag);
void button_selectModel(IGuiComponent*, uint flag);
void gui_entityProcessCommand_Float(IGuiComponent*, float value);
void gui_entityProcessCommand_String(IGuiComponent*, const String&);
void gui_entityProcessCommand_Bool(IGuiComponent*, bool);
void gui_entityProcessCommand_StringChain(IGuiComponent* pComponent, const List<String>& cmds, uint flag);
