#include "EventEngine.h"
#include "../engine/Entity.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Event_TransformChange::Event_TransformChange()
{
	flag = eTransformation::None;
}