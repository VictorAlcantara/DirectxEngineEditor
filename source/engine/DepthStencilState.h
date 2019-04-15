#pragma once

#include "api_DepthStencilState.h"
#include "RenderUtil.h"
#include "../engine/Class.h"

class Renderer;

class DepthStencilState : public ProtectedModule
{
	friend class Renderer;

	public:
		virtual ~DepthStencilState() = default;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		DepthStencilState();

		void EnableDepthWrite(bool);
		void SetDepthOperation(eDepthOperation);

		platform::DepthStencilStateApi* m_depthStencilApiArray;
		uint m_apiCount;
		bool m_allowWrite;
		eDepthOperation m_currentDepthOperation;
};
