#include "Entity_Monster.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/CoreInterface.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "../engine/Path.h"
#include "Entity_Player.h"
#include "AiState.h"
#include "GameGlobal.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	Entity_Monster
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Monster::Entity_Monster() : Entity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_headingVelocity = 7.5f;
	m_desiredHeading = math::AXIS_Z;
	m_active = false;
	m_spawned = false;
}
//-------------------------------------------------------------------
Entity_Monster::~Entity_Monster()
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_Monster::OnShutdown()
{
	g_pScene->Debug_Remove( m_hDebugHeading );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Monster::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorMonster.tga"), false );
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->emissiveIntensity = 0.3f;
	pRenderInfo->ambientIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->visible = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );
	m_hCollision->SetNoclip( true );

	m_hPhysics = AddComponentAs<Component_Physics>();
	m_hPhysics->GetData()->friction = 0.15f;
	m_hPhysics->GetData()->mass = 1.0f;
	m_hPhysics->GetData()->maxVelocity = 15.0f;

	UpdateCollisionInfoFromRenderInfo();

	m_pView = new AiView_Monster(GetHandle());
	m_pView->SetData( gameGlobal::ai::monster::FORCE_MAX, 500.0f );
	m_pView->SetData( gameGlobal::ai::monster::SLASH_DURATION, 0.2f );
	m_pView->SetData( gameGlobal::ai::monster::SLASH_RANGE, 2.0f );
	m_pView->SetData( gameGlobal::ai::monster::CHASE_TIME_MIN, 0.5f );
	m_pView->SetData( gameGlobal::ai::monster::CHASE_TIME_MAX, 1.7f );
	m_pView->SetData( gameGlobal::ai::monster::CHASE_TIME_TURN_MIN, 0.0f );
	m_pView->SetData( gameGlobal::ai::monster::CHASE_TIME_TURN_MAX, 0.5f );

	m_hDebugHeading = g_pScene->Debug_AddLine();
	m_hDebugHeading->color4 = math::normalizeRgb( 244, 66, 66 );
	m_hDebugHeading->visible = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Entity_Monster::Update()
{
	if ( !m_pStateMachine )
		return;

	m_pStateMachine->Update();
	m_pView->Update();

	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);
	pRenderInfo->SetTranslation( m_hPhysics->GetPos() );

	math::Vec3 look = GetHeading();
	if ( !math::isEqual(look * m_desiredHeading, 1.0f) )
	{
		math::Vec3 toDesiredHeading = m_desiredHeading - look;
		toDesiredHeading = math::clampLength( toDesiredHeading, 0.0f, m_headingVelocity * g_pCore->GetTimeSinceLastFrame() );

		look += toDesiredHeading;
		look = look - (math::AXIS_Y * look * math::AXIS_Y);

		if ( !math::isZero(look * look) )
		{
			look = math::normalize( look );
			pRenderInfo->SetRotation( math::matrixOrthogonalLookUp( look, math::AXIS_Y ) );
			m_hDebugHeading->point0 = pRenderInfo->GetTransform().translate.GetPos();
			m_hDebugHeading->point1 = m_hDebugHeading->point0 + GetHeading() * 3.5f;
		}
	}
}
//-------------------------------------------------------------------
//	UpdateCollisionInfo
//-------------------------------------------------------------------
void Entity_Monster::UpdateCollisionInfoFromRenderInfo()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetData();
	m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	m_hCollision->GetCollider(true)->aabbScale = pRenderInfo->GetTransform().GetScale();
}
//-------------------------------------------------------------------
//	HeadTo
//-------------------------------------------------------------------
void Entity_Monster::HeadTo(const math::Vec3& desiredHeading, float force)
{
	if ( math::isZero(desiredHeading * desiredHeading) )
		m_desiredHeading = GetComponentPtrAs<Component_Render>()->GetData()->GetTransform().rotation.GetLook();
	else
	{
		m_desiredHeading = math::normalize( desiredHeading );

		static const float minAngleMove = math::cosDegs( 30.0f );
		if ( m_desiredHeading * GetHeading() > minAngleMove )
			m_hPhysics->AddForce( GetHeading() * force );
	}
}
//-------------------------------------------------------------------
//	GetHeading
//-------------------------------------------------------------------
math::Vec3 Entity_Monster::GetHeading()const
{
	return GetComponentPtrAs<Component_Render>()->GetData()->GetTransform().rotation.GetLook();
}
//-------------------------------------------------------------------
//	StartAi
//-------------------------------------------------------------------
void Entity_Monster::StartAi()
{
	if ( m_pStateMachine )
		return;

	m_pStateMachine = new AiStateMachine(m_pView.As<AiView>());
	m_pStateMachine->ScheduleChangeState( new AiState_Chase );

	m_pTargetSelect = new Process_TargetSelect;
	m_pTargetSelect->SetView( m_pView.As<AiView>() );
	m_pTargetSelect->SetTargetClassTag( Entity_Player::GetClassTagStatic() );

	g_pProcess->Add( m_pTargetSelect.As<Process>() );
}
//-------------------------------------------------------------------
//	StopAi
//-------------------------------------------------------------------
void Entity_Monster::StopAi()
{
	m_pStateMachine = nullptr;
}
//-------------------------------------------------------------------
//	Spawn
//-------------------------------------------------------------------
void Entity_Monster::Spawn()
{
	GetComponentPtrAs<Component_Render>()->GetData()->visible = true;
	GetComponentPtrAs<Component_Collision>()->SetNoclip(false);
	GetComponentPtrAs<Component_Physics>()->GetData()->acceleration = math::Vec3( 0.0f, -9.8f * 0.1f, 0.0f );

	if ( m_active )
		StartAi();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Monster::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_MONSTER );

	if ( !isCommandValid )
	{
		VecGrid gridData;
		math::Vec4 vec4Data;
		String strData;
		bool boolData = true;

		isCommandValid = true;

		if ( pLine->SetQuotedString(command::NAME, strData) )
			SetName( strData );
		else if ( pLine->HasString(command::SPAWN) || pLine->SetBool(command::SPAWN_ON_CREATE, boolData) )
		{
			if ( boolData )
			{
				m_spawned = true;
				Spawn();
			}
		}
		else if ( pLine->SetBool(command::ACTIVE, m_active) )
		{
			if ( m_active )
			{
				if ( m_spawned )
					StartAi();
			}
			else
				StopAi();
		}
		else if ( pLine->SetIntArray(command::GRID_POS, &gridData[0], 3) )
			pRenderInfo->SetTranslation( Grid::ToVec3(gridData) );
		else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
			pRenderInfo->SetScale( Grid::ToVec3(gridData) );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );

	UpdateCollisionInfoFromRenderInfo();
}

/****************************************************************************************
	Process_TargetSelect
*****************************************************************************************/

//-------------------------------------------------------------------
//	SetView
//-------------------------------------------------------------------
void Process_TargetSelect::SetView(SmartPtr<AiView> pView)
{
	m_pView = pView;
}
//-------------------------------------------------------------------
//	SetTargetClassTag
//-------------------------------------------------------------------
void Process_TargetSelect::SetTargetClassTag(const HashString& classTag)
{
	m_classTag = classTag;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_TargetSelect::Update()
{
	if ( m_classTag.IsValid() && m_pView )
	{
		for ( auto it = g_pEntity->GetByClassTag(m_classTag); it; it++ )
		{
			m_pView->SetTarget( *it );
			Exit();
			break;
		}
	}
}
