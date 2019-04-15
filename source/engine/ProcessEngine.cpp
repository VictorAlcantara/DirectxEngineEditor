#include "../engine/Component_Render.h"
#include "../engine/Component_Physics.h"
#include "../engine/Component_Collision.h"

/****************************************************************************************
	Process_UpdateTransform
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_UpdateTransform::Process_UpdateTransform(Component* pOwner)
{
	CHECK( pOwner );
	m_pOwner = pOwner;

	if ( m_pOwner->GetClassRelationship_IsOrHas<Component_Render>() )
		m_flag = eTransformation::Render;
	else if ( m_pOwner->GetClassRelationship_IsOrHas<Component_Collision>() )
		m_flag = eTransformation::Collision;
	else
	{
		m_flag = eTransformation::None;
		ASSERT( "Type does not support transformation updating" );
	}
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_UpdateTransform::Update()
{
	if ( m_flag == eTransformation::Render )
		((Component_Render*)m_pOwner)->UpdateTransform();
	else if ( m_flag == eTransformation::Collision )
		((Component_Collision*)m_pOwner)->UpdateTransform();

	Exit();
}
