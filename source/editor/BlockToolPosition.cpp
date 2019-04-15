//#include "BlockToolPosition.h"
//#include "../engine/Vertex.h"
//#include "../engine/CoreInterface.h"
//#include "Block.h"
//
////-------------------------------------------------------------------
////	Cdtor
////-------------------------------------------------------------------
//BlockToolPosition::BlockToolPosition()
//{
//	m_state = eBlockToolState::Idle;
//}
////-------------------------------------------------------------------
//BlockToolPosition::~BlockToolPosition()
//{
//
//}
////-------------------------------------------------------------------
////	Initialize
////-------------------------------------------------------------------
//void BlockToolPosition::Initialize()
//{
//	for ( uint i = 0; i < ARRAY_COUNT(m_blockPos); i++ )
//	{
//		m_blockPos[i] = new BlockPosition;
//		m_blockPos[i]->Initialize( math::AXES_ENUM[i] );
//	}
//}
////-------------------------------------------------------------------
////	Update
////-------------------------------------------------------------------
//void BlockToolPosition::Update(Block* pBlock, const math::CameraTransform& camera)
//{
//	HandleClick( pBlock, camera );
//	HandleUnclick();
//	HandleToolUse( pBlock, camera );
//
//	if ( pBlock )
//	{
//		for ( uint i = 0; i < ARRAY_COUNT(m_blockPos); i++ )
//			m_blockPos[i]->Update( pBlock, camera );
//	}
//}
////-------------------------------------------------------------------
////	Render
////-------------------------------------------------------------------
//void BlockToolPosition::Render(Block* pBlock, const math::CameraTransform& camera)
//{
//	math::Vec3 lightDir = math::normalize(pBlock->GetPos() - camera.pos);
//
//	for ( uint i = 0; i < ARRAY_COUNT(m_blockPos); i++ )
//		m_blockPos[i]->Render( camera, lightDir );
//}
////-------------------------------------------------------------------
////	IsUsing
////-------------------------------------------------------------------
//bool BlockToolPosition::IsUsing()const
//{
//	return m_state != eBlockToolState::Idle;
//}
////-------------------------------------------------------------------
////	HandleClick
////-------------------------------------------------------------------
//void BlockToolPosition::HandleClick(Block* pBlock, const math::CameraTransform& camera)
//{
//	if ( m_state != eBlockToolState::Idle )
//		return;
//
//	eBlockToolState newState = eBlockToolState::Idle;
//
//	if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_RIGHT) )
//		newState = eBlockToolState::Resize;
//	else if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) )
//		newState = eBlockToolState::Translate;
//
//	if ( newState != eBlockToolState::Idle )
//	{
//		math::Vec3 clickDir = math::mousePosToWorld( camera );
//		uint intersectIdx = 0;
//		float intersectAmount = -1.0f;
//
//		for ( uint i = 0; i < ARRAY_COUNT(m_blockPos); i++ )
//		{
//			float newAmount = m_blockPos[i]->GetClosestCollision(camera.pos, clickDir);
//			if ( intersectAmount < 0.0f || (newAmount > 0.0f && newAmount < intersectAmount) )
//			{
//				intersectIdx = i;
//				intersectAmount = newAmount;
//			}
//		}
//
//		if ( intersectAmount > 0.0f )
//		{
//			g_pInput->LockMouse(true);
//			g_pInput->ShowCursor(false);
//			m_pCurrent = m_blockPos[intersectIdx].Get();
//			m_pCurrent->PrepareUse( pBlock, camera.pos + clickDir * intersectAmount, camera );
//			m_state = newState;
//		}
//	}
//}
////-------------------------------------------------------------------
////	HandleUnclick
////-------------------------------------------------------------------
//void BlockToolPosition::HandleUnclick()
//{
//	// TODO: Add key mapping
//	bool stopToolUse = m_state == eBlockToolState::Resize && g_pInput->IsKeyRelease(eKey::KEY_MOUSE_RIGHT) ||
//		m_state == eBlockToolState::Translate && g_pInput->IsKeyRelease(eKey::KEY_MOUSE_LEFT);
//
//	if ( stopToolUse )
//	{
//		m_state = eBlockToolState::Idle;
//		g_pInput->LockMouse(false);
//		g_pInput->ShowCursor(true);
//		m_pCurrent->EndUse();
//		m_pCurrent = nullptr;
//	}
//}
////-------------------------------------------------------------------
////	HandleToolUse
////-------------------------------------------------------------------
//void BlockToolPosition::HandleToolUse(Block* pBlock, const math::CameraTransform& camera)
//{
//	if ( m_state != eBlockToolState::Idle )
//	{
//		math::Vec2 dist = g_pInput->GetMousePos() - g_pInput->GetMouseLockPos();
//		if ( math::lengthSq(dist) > 0.0f )
//			m_pCurrent->Use( pBlock, dist, camera, m_state );
//	}
//}
