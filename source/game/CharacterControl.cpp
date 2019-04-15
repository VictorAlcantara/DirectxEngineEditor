#include "CharacterControl.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
CharacterControl::CharacterControl()
{

}
//-------------------------------------------------------------------
CharacterControl::~CharacterControl()
{

}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void CharacterControl::Update(uint cmd, bool grounded, const math::Mtx44& orientation)
{
	m_force = math::Vec3( 0.0f );

	if ( !grounded || cmd == CHARACTER_CONTROL_NONE )
		return;

	math::Vec3 forceDir = math::Vec3(0.0f);

	if ( cmd & CHARACTER_CONTROL_FORWARD ) forceDir += orientation.GetLook();
	if ( cmd & CHARACTER_CONTROL_BACKWARD ) forceDir += -orientation.GetLook();
	if ( cmd & CHARACTER_CONTROL_RIGHT ) forceDir += orientation.GetRight();
	if ( cmd & CHARACTER_CONTROL_LEFT ) forceDir += -orientation.GetRight();

	if ( forceDir.x != 0.0f || forceDir.y != 0.0f || forceDir.z != 0.0f )
	{
		forceDir.y = 0.0f;
		forceDir = math::normalize( forceDir );

		m_force = forceDir * m_config.walkForce;
	}
}
//-------------------------------------------------------------------
//	GetForce
//-------------------------------------------------------------------
math::Vec3 CharacterControl::GetForce()const
{
	return m_force;
}
