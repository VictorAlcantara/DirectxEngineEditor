//#include "Entity_Test.h"
//#include "../engine/CoreInterface.h"
//#include "../shared/Command.h"
//#include "../editor/Grid.h"
//
//math::Vec3 initialPos( 0.0f, 3.0f, 5.0f );
//Handle<Debug_Line> hDebugVelocity;
//Handle<Debug_Sphere> hDebug;
//
////-------------------------------------------------------------------
////	Cdtor
////-------------------------------------------------------------------
//Entity_Test::Entity_Test() : Entity(true)
//{
//	RegisterInheritance( "Entity_Test" );
//	m_pCollision = nullptr;
//}
////-------------------------------------------------------------------
//Entity_Test::~Entity_Test()
//{
//
//}
////-------------------------------------------------------------------
////	Shutdown
////-------------------------------------------------------------------
//void Entity_Test::Shutdown()
//{
//	g_pScene->Debug_Remove( hDebug );
//	g_pScene->Debug_Remove( hDebugVelocity );
//}
////-------------------------------------------------------------------
////	Initialize
////-------------------------------------------------------------------
//void Entity_Test::Initialize()
//{
//	hDebug = g_pScene->Debug_AddSphere();
//	hDebugVelocity = g_pScene->Debug_AddLine();
//
//	hDebugVelocity->color4 = math::normalizeRgb( 20, 100, 150 );
//
//	m_movementForce = 10.0f;
//
//	m_pCollision = UseCollision<CollisionInfo_Ellipse>();
//	m_pCollision->hDebugEllipse->visible = true;
//	SetScale( math::Vec3( 0.5f, 1.4f, 0.5f ) );
//
//	UsePhysics( true );
//	GetPhysicsInfo()->friction = 0.3f;
//	GetPhysicsInfo()->mass = 1.0f;
//	GetPhysicsInfo()->acceleration = math::Vec3(0.0f, 0.0f, 0.0f);
//	GetPhysicsInfo()->maxVelocity = 5.0f;
//	GetPhysicsInfo()->pos = initialPos;
//	m_grounded = false;
//
//	m_pCharacterControl = new CharacterControl;
//	m_pCharacterControl->GetConfig().walkForce = 2.5f;
//}
////-------------------------------------------------------------------
////	Update
////-------------------------------------------------------------------
//void Entity_Test::Update()
//{
//	UpdateControl();
//
//	//math::Vec3 force;
//	if ( g_pInput->IsKeyDown(KEY_CTRL) )
//	{
//		if ( g_pInput->IsKeyDown(KEY_R) )
//		{
//			GetPhysicsInfo()->pos = initialPos;
//			GetPhysicsInfo()->velocity = math::Vec3(0.0f);
//			//UseGravity( false );
//		}
//		else if ( g_pInput->IsKeyPress(KEY_G) )
//		{
//			if ( math::isZero(GetPhysicsInfo()->acceleration) )
//				UseGravity( true );
//			else
//				UseGravity( false );
//		}
//	}
//
//	hDebug->pos = GetPhysicsInfo()->pos;
//	hDebug->color4 = math::Vec4(1.0f);
//
//	hDebugVelocity->msg = "entityTest_debugVelocity";
//	hDebugVelocity->point0 = GetPhysicsInfo()->pos;
//	hDebugVelocity->point1 = hDebugVelocity->point0 + GetPhysicsInfo()->velocity;
//	
//	m_grounded = false;
//}
////-------------------------------------------------------------------
////	UpdateControl
////-------------------------------------------------------------------
//void Entity_Test::UpdateControl()
//{
//	uint cmd = CHARACTER_CONTROL_NONE;
//
//	if ( g_pInput->IsKeyDown(KEY_CTRL) )
//	{
//		if ( g_pInput->IsKeyDown(KEY_UP) )
//			cmd |= CHARACTER_CONTROL_FORWARD;
//		if ( g_pInput->IsKeyDown(KEY_DOWN) )
//			cmd |= CHARACTER_CONTROL_BACKWARD;
//		if ( g_pInput->IsKeyDown(KEY_LEFT) )
//			cmd |= CHARACTER_CONTROL_LEFT;
//		if ( g_pInput->IsKeyDown(KEY_RIGHT) )
//			cmd |= CHARACTER_CONTROL_RIGHT;
//	}
//
//	m_pCharacterControl->Update( cmd, m_grounded, m_orientation );
//	math::Vec3 force = m_pCharacterControl->GetForce();
//	if ( !math::isZero(force) )
//		g_pPhysics->AddForce( force, GetId() );
//}
////-------------------------------------------------------------------
////	OnPhysicsUpdate
////-------------------------------------------------------------------
//void Entity_Test::OnPhysicsUpdate(Contact* contacts, uint contactCount)
//{
//	hDebug->pos = GetPhysicsInfo()->pos;
//
//	const float groundAngleCos = math::cosDegs( 10.0f );
//
//	for ( uint i = 0; i < contactCount; i++ )
//	{
//		if ( contacts[i].surfaceNormal * math::AXIS_Y > groundAngleCos )
//		{
//			m_grounded = true;
//			break;
//		}
//	}
//}
////-------------------------------------------------------------------
////	SetScale
////-------------------------------------------------------------------
//void Entity_Test::SetScale(const math::Vec3& scale)
//{
//	m_pCollision->scale = math::Vec3(1.0f);
//	m_pCollision->scale = scale;
//	hDebug->scale = m_pCollision->scale;
//}
////-------------------------------------------------------------------
////	ProcessCommand
////-------------------------------------------------------------------
//void Entity_Test::ProcessCommand(const ScriptLine* pLine)
//{
//	VecGrid vecGrid;
//	math::Vec3 direction;
//
//	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
//	{
//		GetPhysicsInfo()->pos = Grid::ToVec3( vecGrid );
//	}
//	else if ( pLine->SetIntArray(command::GRID_SCALE, &vecGrid[0], 3) )
//	{
//		SetScale( Grid::ToVec3(vecGrid) );
//	}
//	else if ( !pLine->HasString( command::ENTITY_PLAYER_SPAWN ) )
//	{
//		ASSERT( "Invalid command" );
//	}
//}
////-------------------------------------------------------------------
////	UseGravity
////-------------------------------------------------------------------
//void Entity_Test::UseGravity(bool use)
//{
//	GetPhysicsInfo()->acceleration = math::Vec3( 0.0f, -9.8f * (use ? 0.1f: 0.0f), 0.0f );
//}
