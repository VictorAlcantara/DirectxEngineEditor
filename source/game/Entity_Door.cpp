#include "Entity_Door.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/CoreInterface.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "GameCollision.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	Entity_Door
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Door::Entity_Door() : Entity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_locked = false;
	SetDuration( 0.2f );
	m_state = eDoorState::Closed;
	m_openAmount = 0.9f;
	m_duration = 1.0f;
	m_closeTimer = 0.0f;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Door::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Opaque;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );

	UpdateRenderInfo();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Entity_Door::Update()
{
	m_openedPos = m_closedPos + math::scale( m_openDirection, m_hCollision->GetCollider()->aabbScale ) * 2.0f * m_openAmount;

	math::Vec3 pos = m_pos;
	bool updateTransform = false;

	if ( m_state == eDoorState::Opening )
	{
		math::Vec3 toOpened = m_openedPos - m_closedPos;
		math::Vec3 velocity = toOpened / m_duration;

		float d = math::projectPointOnLine( pos + velocity * g_pCore->GetTimeSinceLastFrame(), m_closedPos, m_openedPos );
		d = math::clamp( d, 0.0f, 1.0f );

		if ( math::isEqual(d, 1.0f) )
		{
			m_pos = m_openedPos;
			m_state = eDoorState::Opened;
		}
		else
			m_pos = m_closedPos + toOpened * d;

		updateTransform = true;
	}
	else if ( m_state == eDoorState::Closing )
	{
		math::Vec3 toClosed = m_closedPos - m_openedPos;
		math::Vec3 velocity = toClosed / m_duration;

		float d = math::projectPointOnLine( pos + velocity * g_pCore->GetTimeSinceLastFrame(), m_openedPos, m_closedPos );
		d = math::clamp( d, 0.0f, 1.0f );

		if ( math::isEqual(d, 1.0f) )
		{
			m_pos = m_closedPos;
			m_state = eDoorState::Opened;
		}
		else
			m_pos = m_openedPos + toClosed * d;

		updateTransform = true;
	}
	else if ( m_state == eDoorState::Opened )
	{
		if ( m_closeTimer >= 0.0f )
		{
			m_closeTimer -= g_pCore->GetTimeSinceLastFrame();
			if ( m_closeTimer < 0.0f )
				Close();
		}
	}

	if ( updateTransform )
	{
		if ( !math::isEqual(pos, m_pos) )
			UpdateRenderInfo();
	}
}
//-------------------------------------------------------------------
//	Open
//-------------------------------------------------------------------
void Entity_Door::Open()
{
	if ( m_locked )
		return;

	m_state = eDoorState::Opening;
	m_closeTimer = m_duration;
}
//-------------------------------------------------------------------
//	Close
//-------------------------------------------------------------------
void Entity_Door::Close()
{
	m_state = eDoorState::Closing;
	m_closeTimer = -1.0f;
}
//-------------------------------------------------------------------
//	UpdateCollisionInfo
//-------------------------------------------------------------------
void Entity_Door::UpdateRenderInfo()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetData(true);
	pRenderInfo->SetTranslation( m_pos );
	pRenderInfo->SetScale( m_hCollision->GetCollider()->aabbScale );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Door::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>();
	bool updateTransform = false;

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_DOOR );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );

	isCommandValid |= pLine->SetFloatArray( command::OPEN_DIRECTION, &m_openDirection[0], 3, false );
	isCommandValid |= pLine->SetFloat( command::OPEN_AMOUNT, m_openAmount );
	isCommandValid |= pLine->SetFloat( command::DOOR_DURATION, m_duration );

	isCommandValid |= pLine->SetBool( command::LOCKED, m_locked );

	if ( !isCommandValid )
	{
		String strData;
		float floatData;
		VecGrid gridData;
		math::Vec2 vec2Data;
		math::Vec4 vec4Data;

		isCommandValid = true;

		if ( pLine->SetFloat( command::TEXTURE_POS_U, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexTranslate();
			texData.x = floatData;
			pRenderInfo->SetTexTranslate( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_POS_V, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexTranslate();
			texData.y = floatData;
			pRenderInfo->SetTexTranslate( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_SCALE_U, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexScale();
			texData.x = floatData;
			pRenderInfo->SetTexScale( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_SCALE_V, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexScale();
			texData.y = floatData;
			pRenderInfo->SetTexScale( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_ROTATION, floatData ) )
			pRenderInfo->SetTexRotationDegs( floatData );
		else if ( pLine->SetFloatArray(command::TEXTURE_POS, &vec4Data[0], 2, false) )
			pRenderInfo->SetTexTranslate( vec4Data.Xy() );
		else if ( pLine->SetFloatArray(command::TEXTURE_SCALE, &vec4Data[0], 2, false) )
			pRenderInfo->SetTexScale ( vec4Data.Xy() );
		else if ( pLine->SetIntArray(command::GRID_POS, &gridData[0], 3) )
		{
			math::Vec3 pos = Grid::ToVec3(gridData);
			m_pos = pos;
			m_closedPos = pos;
			updateTransform = true;
		}
		else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
		{
			m_hCollision->GetCollider(true)->aabbScale = Grid::ToVec3(gridData);
			updateTransform = true;
		}
		else if ( pLine->HasString(command::TEXTURE_DIFFUSE) )
			pRenderInfo->hDiffuseMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->SetFloatArray( command::OPEN_DIRECTION, &m_openDirection[0], 3, false ) )
			m_openedPos = m_closedPos + math::scale( m_openDirection, m_hCollision->GetCollider()->aabbScale ) * 1.9f;
		else if ( pLine->SetQuotedString( command::NAME, strData ) )
			SetName( strData );
		else if ( pLine->HasString( command::OPEN ) )
			Open();
		else if ( pLine->HasString( command::OPEN_STAY ) )
		{
			Open();
			m_closeTimer = -1.0f;
		}
		else if ( pLine->HasString( command::CLOSE ) )
			Close();
		else if ( pLine->SetQuotedString(command::LOCK_TEXT, strData) )
		{
			if ( m_lockText.Length() > 0 )
				m_lockText += "\n";
			m_lockText += strData;
		}
		else if ( pLine->HasString(command::LOCK_TEXT_CLEAR) )
			m_lockText = "";
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );

	if ( updateTransform )
		UpdateRenderInfo();
}
