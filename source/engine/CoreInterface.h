#pragma once

#include "Core.h"
#include "Renderer.h"
#include "Input.h"
#include "ApplicationCoreInterface.h"
#include "EventDispatcher.h"
#include "ResourceManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "EntityManager.h"
#include "../engine/Scene.h"
#include "CollisionManager.h"
#include "Physics.h"
#include "ProcessManager.h"
#include "KeyMap.h"

#define g_pCore Core::GetInstance()
#define g_pRenderer g_pCore->GetRenderer()
#define g_pInput g_pCore->GetInput()
#define g_pApplication g_pCore->GetApplication()
#define g_pEvent g_pCore->GetEventDispatcher()
#define g_pResource g_pCore->GetResourceMngr()
#define g_pEntity g_pCore->GetEntityMngr()
#define g_pScene g_pCore->GetScene()
#define g_pCollision g_pCore->GetCollisionMngr()
#define g_pPhysics g_pCore->GetPhysics()
#define g_pProcess g_pCore->GetProcessMngr()
#define g_pKeyMap g_pCore->GetKeyMap()

#define PROFILE_START(name) { static const HashString __profile(name); g_pCore->ProfileStart( __profile ); }
#define PROFILE_END(name) { static const HashString __profile(name); g_pCore->ProfileEnd( __profile ); }
