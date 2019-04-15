//#include "BlockToolSize.h"
//#include "../engine/Vertex.h"
//#include "../engine/CoreInterface.h"
//#include "../engine/Debug.h"
//
////-------------------------------------------------------------------
////	Cdtor
////-------------------------------------------------------------------
//BlockToolSize::BlockToolSize()
//{
//	m_using = false;
//	m_pCurrent = nullptr;
//}
////-------------------------------------------------------------------
//BlockToolSize::~BlockToolSize()
//{
//
//}
////-------------------------------------------------------------------
////	Initialize
////-------------------------------------------------------------------
//void BlockToolSize::Initialize()
//{
//	SimplePtr<math::AxisInfo> grows[][3] = { 
//		{ new math::AxisInfo(math::eAxis::X, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::NEGATIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::NEGATIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::POSITIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::POSITIVE) },
//
//		{ new math::AxisInfo(math::eAxis::X, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Y, math::POSITIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Y, math::NEGATIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Y, math::POSITIVE) },
//		{ new math::AxisInfo(math::eAxis::X, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Y, math::NEGATIVE) },
//		
//		{ new math::AxisInfo(math::eAxis::Y, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::NEGATIVE) },
//		{ new math::AxisInfo(math::eAxis::Y, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::NEGATIVE) },
//		{ new math::AxisInfo(math::eAxis::Y, math::POSITIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::POSITIVE) },
//		{ new math::AxisInfo(math::eAxis::Y, math::NEGATIVE), nullptr, new math::AxisInfo(math::eAxis::Z, math::POSITIVE) },
//	};
//
//	for ( uint i = 0; i < ARRAY_COUNT(m_resizes); i++ )
//	{
//		m_resizes[i] = new BlockResize();
//		m_resizes[i]->Initialize( grows[i][0].Get(), grows[i][1].Get(), grows[i][2].Get() );
//	}
//}
////-------------------------------------------------------------------
////	Update
////-------------------------------------------------------------------
//void BlockToolSize::Update(Block* pBlock, const math::CameraTransform& camera)
//{
//	for ( uint i = 0; i < ARRAY_COUNT(m_resizes); i++ )
//		m_resizes[i]->Update( pBlock, camera );
//
//	HandleClick( pBlock, camera );
//	HandleUnclick();
//	HandleToolUse( pBlock );
//}
////-------------------------------------------------------------------
////	Render
////-------------------------------------------------------------------
//void BlockToolSize::Render(Block* pBlock, const math::CameraTransform& camera)
//{
//	math::Vec3 lightDir( -1.0f, -1.0f, -1.0f );
//	math::Mtx44 rotateLight = math::matrixOrthogonalLookUp( camera.pos - pBlock->GetPos(), camera.up );
//	lightDir = (math::Vec4(lightDir, 0.0f) * rotateLight).Xyz();
//
//	for ( uint i = 0; i < ARRAY_COUNT(m_resizes); i++ )
//		m_resizes[i]->Render( camera, math::normalize(lightDir) );
//}
////-------------------------------------------------------------------
////	IsToolInUse
////-------------------------------------------------------------------
//bool BlockToolSize::IsUsing()const
//{
//	return m_using;
//}
////-------------------------------------------------------------------
////	HandleClick
////-------------------------------------------------------------------
//void BlockToolSize::HandleClick(Block* pBlock, const math::CameraTransform& camera)
//{
//	if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) )
//	{
//		uint resizeClosestIdx = 0;
//		float resizeClosestAmount = -1.0f;
//
//		for ( uint i = 0; i < ARRAY_COUNT(m_resizes); i++ )
//		{
//			float newAmount = m_resizes[i]->GetCollisionDistance(pBlock, camera);
//			if ( resizeClosestAmount < 0.0f || (newAmount < resizeClosestAmount && newAmount > 0.0f) )
//			{
//				resizeClosestIdx = i;
//				resizeClosestAmount = newAmount;
//			}
//		}
//
//		if ( resizeClosestAmount > 0.0f )
//		{
//			m_using = true;
//			g_pInput->ShowCursor( false );
//			g_pInput->LockMouse( true );
//			m_pCurrent = m_resizes[resizeClosestIdx].Get();
//			m_pCurrent->LockToAxis( pBlock, camera );
//		}
//	}
//}
////-------------------------------------------------------------------
////	HandleUnclick
////-------------------------------------------------------------------
//void BlockToolSize::HandleUnclick()
//{
//	if ( m_using && g_pInput->IsKeyUp( eKey::KEY_MOUSE_LEFT ) )
//	{
//		m_using = false;
//		m_pCurrent->UnlockAxis();
//		g_pInput->ShowCursor( true );
//		g_pInput->LockMouse( false );
//	}
//}
////-------------------------------------------------------------------
////	HandleToolUse
////-------------------------------------------------------------------
//void BlockToolSize::HandleToolUse(Block* pBlock)
//{
//	if ( m_using )
//	{
//		math::Vec2 dist = g_pInput->GetMousePos() - g_pInput->GetMouseLockPos();
//		m_pCurrent->Use( pBlock, dist );
//	}
//}
