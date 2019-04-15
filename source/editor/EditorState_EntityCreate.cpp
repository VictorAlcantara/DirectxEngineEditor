#include "EditorState_EntityCreate.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "EditorState_Idle.h"
#include "../shared/FirstPersonCamera.h"
#include "EditorEntity.h"
#include "../editor/Grid.h"
#include "EditorEvent.h"
#include "RenderInfoEditor.h"
#include "EditorEntity_PointLight.h"
#include "EditorEntity_SpotLight.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorState_EntityCreate::EditorState_EntityCreate()
{
	m_entityAlphaDefault = 1.0f;
	m_entityMaterialDefault = eMaterial::Opaque;
	m_editorEntityType = eEditorEntityType::Unknown;
	m_castShadowDefault = false;
}
//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void EditorState_EntityCreate::Enter()
{
	g_pEditor->GetGui()->HideAll();

	m_gradient.SetInterval( 0.2f, 0.3f );
	m_gradient.SetDuration( 1.0f );

	sendEvent_EntityUnselect();

	CreateEntityGhost();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorState_EntityCreate::Update()
{
	m_gradient.Update();

	if ( m_pEntityGhost )
	{
		SetAlpha( m_gradient.Get() );

		m_pEntityGhost->SetGridPos( GetEntityGridPos( g_pInput->IsKeyDown(KEY_CTRL) ) );
	}

	if ( g_pKeyMap->CanPerform(editorGlobal::action::ENTITY_SELECT) )
	{
		if ( m_pEntityGhost )
		{
			RestoreRenderData();
			m_pEntityGhost = nullptr;
		}

		if ( m_editorEntityType == eEditorEntityType::AmbientLight )
			m_pOwner->ChangeState( new EditorState_Idle );
		else
			CreateEntityGhost();
	}
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::ESCAPE) )
	{
		m_pOwner->ChangeState( new EditorState_Idle );
	}
}
//-------------------------------------------------------------------
//	Exit
//-------------------------------------------------------------------
void EditorState_EntityCreate::Exit()
{
	if ( m_pEntityGhost )
	{
		if ( m_pEntityGhost->GetType() == eEditorEntityType::AmbientLight )
			RestoreRenderData();
		else
			g_pEntity->Remove( m_pEntityGhost->GetId() );
	}

	m_pEntityGhost = nullptr;
}
//-------------------------------------------------------------------
//	CreateEntityGhost
//-------------------------------------------------------------------
void EditorState_EntityCreate::CreateEntityGhost()
{
	m_pEntityGhost = (EditorEntity*)g_pEditor->EntityCreate( m_editorEntityType ).GetPtr();

	auto pRenderInfo = GetRenderInfo( *m_pEntityGhost, false );

	m_pEntityGhost->SetGridPos( GetEntityGridPos(false) );
	m_entityAlphaDefault = pRenderInfo->color4.w;
	m_entityMaterialDefault = pRenderInfo->material;
	m_castShadowDefault = pRenderInfo->castShadow;

	m_gradient.Reset();
}
//-------------------------------------------------------------------
//	GetEntityGridPos
//-------------------------------------------------------------------
VecGrid EditorState_EntityCreate::GetEntityGridPos(bool attachToOtherEntities)
{
	math::CameraTransform camera = g_pEditor->GetCamera()->GetCameraTransform();
	math::Vec3 look = math::mousePosToWorld( camera );
	math::Vec3 pointClosestAxis;
	math::Vec3 entityPos;
	EditorEntity* pClosestEntity = nullptr;
	float closestEntityT = 0.0f;

	if ( attachToOtherEntities )
		pClosestEntity = (EditorEntity*)g_pEditor->GetEntityAtRayIntersection( camera.pos, look, m_pEntityGhost->GetId(), &closestEntityT ).GetPtrOrNull();

	if ( pClosestEntity && m_pEntityGhost )
	{
		math::Vec3 collisionPoint = camera.pos + look * closestEntityT;
		math::Vec3 closestEntityToCollision = collisionPoint - pClosestEntity->GetPos();
		pointClosestAxis = math::pointClosestAxis( closestEntityToCollision, pClosestEntity->GetScale() );
		entityPos = collisionPoint + math::scale( pointClosestAxis, m_pEntityGhost->GetScale() );
	}
	else
	{
		entityPos = camera.pos + look * 5.0f;
	}

	VecGrid gridPos = entityPos * (float)editorGlobal::grid::UNIT_SIZE_ONE;
	return VecGrid( gridPos );
}
//-------------------------------------------------------------------
//	SetAlpha
//-------------------------------------------------------------------
void EditorState_EntityCreate::SetAlpha(float a)
{
	auto pRenderInfo = GetRenderInfo( *m_pEntityGhost, false );
	if ( m_entityMaterialDefault != eMaterial::Refraction )
		pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->color4.w = a;
	pRenderInfo->castShadow = false;
}
//-------------------------------------------------------------------
//	RestoreRenderData
//-------------------------------------------------------------------
void EditorState_EntityCreate::RestoreRenderData()
{
	auto pRenderInfo = GetRenderInfo( *m_pEntityGhost, false );
	pRenderInfo->material = m_entityMaterialDefault;
	pRenderInfo->color4.w = m_entityAlphaDefault;
	pRenderInfo->castShadow = m_castShadowDefault;
}
//-------------------------------------------------------------------
//	GetRenderInfo
//-------------------------------------------------------------------
RenderInfo_EditorEntity* EditorState_EntityCreate::GetRenderInfo(Entity& entity, bool updateTransform)
{
	auto pCmpRender = entity.GetComponentPtrAs<Component_Render>();

	CHECK( pCmpRender );

	if ( pCmpRender )
		return pCmpRender->GetDataAs<RenderInfo_EditorEntity>(updateTransform);

	return nullptr;
}
