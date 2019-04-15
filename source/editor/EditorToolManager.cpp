#include "EditorToolManager.h"
#include "../engine/CoreInterface.h"
#include "EditorEvent.h"
#include "EditorEntity.h"
#include "../engine/Effect.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "EditorEntity_Model.h"
#include "EditorEntity_Refraction.h"
#include "EditorEntity_Block.h"
#include "EditorEntity_Door.h"


#define TOOL_COUNT ARRAY_COUNT(m_editorTools)

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorToolManager::EditorToolManager()
{
	m_editorToolAction = eEditorToolAction::None;
	m_initialized = false;
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
EditorToolManager::~EditorToolManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EditorToolManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_pEffectLightDir4->Shutdown();
	m_pEffectLightDir4 = nullptr;

	m_pEffectOneColor->Shutdown();
	m_pEffectOneColor = nullptr;

	for ( uint i = 0; i < TOOL_COUNT; i++ )
	{
		m_editorTools[i]->Shutdown();
		m_editorTools[i] = nullptr;
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EditorToolManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	for ( uint i = 0; i < TOOL_COUNT; i++ )
	{
		m_editorTools[i] = new EditorTool;
		m_editorTools[i]->Initialize( math::AXES_ENUM[i] );
	}

	m_pEffectOneColor = new Effect_Color;
	m_pEffectOneColor->Initialize();
	m_pEffectOneColor->GetConfig().renderer.blendMode = eBlendMode::Disabled;
	m_pEffectOneColor->GetConfig().renderer.enableDepthWrite = false;
	m_pEffectOneColor->GetConfig().renderer.depthOperation = eDepthOperation::GreaterEqual;

	m_pEffectLightDir4 = new Effect_LightDir4;
	m_pEffectLightDir4->Initialize();
	m_pEffectLightDir4->GetConfig().renderer.blendMode = eBlendMode::Disabled;
	m_pEffectLightDir4->GetConfig().renderer.depthOperation = eDepthOperation::LessEqual;
	m_pEffectLightDir4->GetConfig().SetLightDir( 0, editorGlobal::tool::LIGHT0_DIR );
	m_pEffectLightDir4->GetConfig().SetLightDir( 1, editorGlobal::tool::LIGHT1_DIR );
	m_pEffectLightDir4->GetConfig().SetLightColor( 0, editorGlobal::tool::LIGHT0_COLOR );
	m_pEffectLightDir4->GetConfig().SetLightColor( 1, editorGlobal::tool::LIGHT1_COLOR );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorToolManager::Update(const math::CameraTransform& camera)
{
	if ( m_hEntitySelected.IsValid() )
	{
		HandleClick( camera );
		HandleUnclick();
		HandleToolUse( camera );

		for ( uint i = 0; i < TOOL_COUNT; i++ )
		{
			m_editorTools[i]->SetCameraTransform( camera );
			m_editorTools[i]->Update();
		}
	}
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void EditorToolManager::Render(const math::CameraTransform& camera)
{
	if ( m_hEntitySelected.IsValid() )
	{
		RenderPassOccluded( camera );
		RenderPassNormal( camera );
	}
}
//-------------------------------------------------------------------
//	RenderPassGray
//-------------------------------------------------------------------
void EditorToolManager::RenderPassOccluded(const math::CameraTransform& camera)
{
	for ( uint i = 0; i < TOOL_COUNT; i++ )
	{
		EditorTool* pEditorTool = m_editorTools[i].Get();

		if ( m_editorToolAction != eEditorToolAction::None && !pEditorTool->IsUsing() )
			continue;

		const RenderInfo_EditorTool& renderInfo = pEditorTool->GetRenderInfo();

		math::Vec4 color = pEditorTool->IsUsing() ? renderInfo.occlusionPass.selectedColor4 : renderInfo.occlusionPass.unselectedColor4;

		m_pEffectOneColor->GetConfig().color4 = color;
		m_pEffectOneColor->Render( renderInfo.hModel, renderInfo.GetTransform().GetWorld(), camera.view, camera.proj );
	}
}
//-------------------------------------------------------------------
//	RenderPassNormal
//-------------------------------------------------------------------
void EditorToolManager::RenderPassNormal(const math::CameraTransform& camera)
{
	for ( uint i = 0; i < TOOL_COUNT; i++ )
	{
		EditorTool* pEditorTool = m_editorTools[i].Get();

		if ( m_editorToolAction != eEditorToolAction::None && !pEditorTool->IsUsing() )
			continue;

		const RenderInfo_EditorTool& renderInfo = pEditorTool->GetRenderInfo();

		math::Vec4 diffuse = pEditorTool->IsUsing() ? renderInfo.normalPass.selectedDiffuse4 : renderInfo.normalPass.unselectedDiffuse4;
		math::Vec4 ambient = pEditorTool->IsUsing() ? renderInfo.normalPass.selectedAmbient4 : renderInfo.normalPass.unselectedAmbient4;

		m_pEffectLightDir4->GetConfig().ambientColor4 = ambient;
		m_pEffectLightDir4->GetConfig().diffuseColor4 = diffuse;
		m_pEffectLightDir4->Render( renderInfo.hModel, renderInfo.GetTransform().GetWorld(), camera.view, camera.proj );
	}
}
//-------------------------------------------------------------------
//	Attach
//-------------------------------------------------------------------
void EditorToolManager::Attach(Handle<Entity> hEditorEntity)
{
	if (  hEditorEntity && hEditorEntity->GetClassRelationship<EditorEntity>() != eClassRelationship::Inherits )
	{
		ASSERT( "Attempt to attach non editor entity to tool" );
		m_hEntitySelected = Handle<Entity>();
		return;
	}

	m_hEntitySelected = hEditorEntity;

	for ( uint i = 0; i < TOOL_COUNT; i++ )
		m_editorTools[i]->Attach( hEditorEntity );
}
//-------------------------------------------------------------------
//	Detach
//-------------------------------------------------------------------
void EditorToolManager::Detach()
{
	Attach( Handle<Entity>() );
}
//-------------------------------------------------------------------
//	IsUsing
//-------------------------------------------------------------------
bool EditorToolManager::IsUsing()const
{
	return m_editorToolAction != eEditorToolAction::None;
}
//-------------------------------------------------------------------
//	HandleClick
//-------------------------------------------------------------------
void EditorToolManager::HandleClick(const math::CameraTransform& camera)
{
	if ( m_editorToolAction != eEditorToolAction::None || g_pEditor->GetGui()->IsCursorOnTopAnyPage() )
		return;

	eEditorToolAction newState = eEditorToolAction::None;

	if ( !HasSpecificToolAction( &newState ) )
	{
		if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_RIGHT) )
		{
			CHECK( !(m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() == (TOOL_ALLOW_RESIZE | TOOL_ALLOW_ROTATE)) )

			if ( m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & TOOL_ALLOW_RESIZE )
				newState = eEditorToolAction::Resize;
			else if ( m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & TOOL_ALLOW_ROTATE )
				newState = eEditorToolAction::Rotate;
		}
		else if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) && (m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & TOOL_ALLOW_TRANSLATE) )
			newState = eEditorToolAction::Translate;
	}

	if ( newState != eEditorToolAction::None )
	{
		math::Vec3 clickDir = math::mousePosToWorld( camera );
		uint intersectIdx = 0;
		float intersectAmount = -1.0f;

		for ( uint i = 0; i < TOOL_COUNT; i++ )
		{
			float newAmount = m_editorTools[i]->GetClosestCollision(camera.pos, clickDir);
			if ( (newAmount > 0.0f && newAmount < intersectAmount) || intersectAmount < 0.0f )
			{
				intersectIdx = i;
				intersectAmount = newAmount;
			}
		}

		if ( intersectAmount > 0.0f )
		{
			m_editorToolAction = newState;
			g_pInput->LockMouse(true);
			g_pInput->ShowCursor(false);
			m_pCurrent = m_editorTools[intersectIdx].Get();
			m_pCurrent->PrepareUse( camera.pos + clickDir * intersectAmount, m_editorToolAction );
		}
	}
}
//-------------------------------------------------------------------
//	HasSpecificToolAction
//-------------------------------------------------------------------
bool EditorToolManager::HasSpecificToolAction(eEditorToolAction* pOutAction)const
{
	eEditorState state = g_pEditor->GetCurrentState()->GetType();

	switch ( state )
	{
		case eEditorState::PickAxis:
			CHECK( m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & TOOL_ALLOW_PICK_AXIS );

			if ( (g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) || g_pInput->IsKeyPress(eKey::KEY_MOUSE_RIGHT)) &&
				(m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & TOOL_ALLOW_PICK_AXIS) )
			{
				if ( pOutAction )
					*pOutAction = eEditorToolAction::PickAxis;
			
				return true;
			}
			break;
	};

	if ( m_hEntitySelected.GetAs<EditorEntity>().GetClassRelationship_Is<EditorEntity_Model>() )
	{
		CHECK( (m_hEntitySelected.GetAs<EditorEntity>().GetToolActionFlag() & (TOOL_ALLOW_RESIZE | TOOL_ALLOW_ROTATE | TOOL_ALLOW_TRANSLATE)) == (TOOL_ALLOW_RESIZE | TOOL_ALLOW_ROTATE | TOOL_ALLOW_TRANSLATE) );

		if ( pOutAction )
		{
			if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_RIGHT) )
			{
				if ( g_pInput->IsKeyDownOrPress(KEY_R) )
					*pOutAction = eEditorToolAction::Rotate;
				else
					*pOutAction = eEditorToolAction::Resize;
			}
			else if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) )
				*pOutAction = eEditorToolAction::Translate;
		}

		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	HandleUnclick
//-------------------------------------------------------------------
void EditorToolManager::HandleUnclick()
{
	bool stopToolUse = m_editorToolAction != eEditorToolAction::None && 
		(g_pInput->IsKeyRelease(eKey::KEY_MOUSE_LEFT) || g_pInput->IsKeyRelease(eKey::KEY_MOUSE_RIGHT));

	if ( stopToolUse )
	{
		m_editorToolAction = eEditorToolAction::None;
		g_pInput->LockMouse(false);
		g_pInput->ShowCursor(true);
		m_pCurrent->EndUse();
		m_pCurrent = nullptr;
	}
}
//-------------------------------------------------------------------
//	HandleToolUse
//-------------------------------------------------------------------
void EditorToolManager::HandleToolUse(const math::CameraTransform& camera)
{
	if ( m_editorToolAction != eEditorToolAction::None )
	{
		math::Vec2 dist = g_pInput->GetMousePos() - g_pInput->GetMouseLockPos();
		if ( m_pCurrent->Use(dist) )
		{
			m_hEntitySelected.GetAs<EditorEntity>().OnToolUse();
		}
	}
	else
	{
		HandleAdjustToGrid();
	}
}
//-------------------------------------------------------------------
//	HandleAdjustToGrid
//-------------------------------------------------------------------
void EditorToolManager::HandleAdjustToGrid()
{
	if ( g_pInput->IsKeyDown(KEY_CTRL) && g_pInput->IsKeyPress(KEY_P) )
	{
		if ( m_hEntitySelected && 
			(
				m_hEntitySelected->GetClassRelationship_IsOrHas<EditorEntity_Block>() || 
				m_hEntitySelected->GetClassRelationship_IsOrHas<EditorEntity_Door>() ||
				m_hEntitySelected->GetClassRelationship_IsOrHas<EditorEntity_Refraction>()
			) 
		)
		{
			EditorEntity* pEntity = m_hEntitySelected->Convert<EditorEntity>();
			VecGrid pos = pEntity->GetGridPos();
			VecGrid scale = pEntity->GetGridScale();
			VecGrid newScale = scale;
			VecGrid newPos = pos;
			int unitsOnLevel = g_pEditor->GetGrid()->GetUnitCurrentLevel();

			newPos.x -= pos.x % unitsOnLevel;
			newPos.y -= pos.y % unitsOnLevel;
			newPos.z -= pos.z % unitsOnLevel;
			newScale.x += g_pEditor->GetGrid()->SnapToGrid( scale.x, math::POSITIVE, unitsOnLevel );
			newScale.y += g_pEditor->GetGrid()->SnapToGrid( scale.y, math::POSITIVE, unitsOnLevel );
			newScale.z += g_pEditor->GetGrid()->SnapToGrid( scale.z, math::POSITIVE, unitsOnLevel );

			pEntity->SetGridScale( newScale );
			pEntity->SetGridPos( newPos, true );
		}
	}
}

#undef TOOL_COUNT
