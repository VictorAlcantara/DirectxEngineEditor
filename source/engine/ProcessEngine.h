#pragma once

#include "../engine/Process.h"
#include "../engine/EngineGlobal.h"

class Component;

/****************************************************************************************
	Process_UpdateTransform
*****************************************************************************************/

class Process_UpdateTransform : public Process
{
	public:
		Process_UpdateTransform(Component* pOwner);
		virtual ~Process_UpdateTransform() = default;

	protected:
		void Update()override final;

	private:
		Component* m_pOwner;
		eTransformation m_flag;
};
