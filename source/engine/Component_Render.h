#pragma once

#include "../engine/Component.h"
#include "../engine/RenderInfo.h"
#include "../engine/Pointer.h"
#include "../engine/Event.h"
#include "ProcessEngine.h"

/****************************************************************************************
	Component_Render
*****************************************************************************************/

class Component_Render : public Component
{
	CLASS_TAG( Component_Render );

	public:
		Component_Render();
		virtual ~Component_Render() = default;

		void SetData(SmartPtr<RenderInfo>&);

		void UpdateTransform();

		RenderInfo* GetData(bool updateTransform = false);
		const RenderInfo* GetData()const;

		template <class RENDER_INFO_TYPE>
		RENDER_INFO_TYPE* GetDataAs(bool updateTransform = false);
		template <class RENDER_INFO_TYPE>
		const RENDER_INFO_TYPE* GetDataAs()const;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SmartPtr<RenderInfo> m_pData;
		SmartPtr<Process_UpdateTransform> m_pProcessUpdateTransform;
};

//-------------------------------------------------------------------
//	GetDataAs
//-------------------------------------------------------------------
template <class RENDER_INFO_TYPE>
RENDER_INFO_TYPE* Component_Render::GetDataAs(bool updateTransform)
{
	CHECK( !m_pData || m_pData->GetType() == RENDER_INFO_TYPE::GetTypeStatic() );
	
	if ( m_pData && m_pData->GetType() == RENDER_INFO_TYPE::GetTypeStatic() )
		return (RENDER_INFO_TYPE*)GetData(updateTransform);
	return nullptr;
}
//-------------------------------------------------------------------
template <class RENDER_INFO_TYPE>
const RENDER_INFO_TYPE* Component_Render::GetDataAs()const
{
	CHECK( !m_pData || m_pData->GetType() == RENDER_INFO_TYPE::GetTypeStatic() );
	
	if ( m_pData && m_pData->GetType() == RENDER_INFO_TYPE::GetTypeStatic() )
		return (const RENDER_INFO_TYPE*)GetData();
	return nullptr;
}
