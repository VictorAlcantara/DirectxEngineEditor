#pragma once

#include "Type.h"

class IListBridge
{
	public:
		virtual void OnAddNode() = 0;
		virtual void OnRemoveNode(uint id) = 0;
};
