#include "GuiPage.h"
#include "../engine/TableHashFunction.h"
#include "EditorGlobal.h"
#include "../engine/CoreInterface.h"
#include "../engine/Debug.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiPage::GuiPage(const HashString& name) : m_components(500, tableHashFunc_Uint)
{
	if ( !name.IsValid() )
		THROW( "GuiPage::ctor() invalid name" );

	m_name = name;
	m_animationTime.SetDuration( editorGlobal::guiPage::TRANSITION_ANIMATION_DURATION );
	m_animationTime.SetInterval( 0.0f, 1.0f );
	m_state = eGuiPageState::Showing;
}
//-------------------------------------------------------------------
GuiPage::~GuiPage()
{
	for ( auto it = m_components.GetIterator(); it; it++ )
		(*it)->Shutdown();

	m_components.Clear();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GuiPage::Update()
{
	if ( m_state == eGuiPageState::Hiding || m_state == eGuiPageState::Showing )
	{
		m_animationTime.Update();

		if ( m_animationTime.HasCompletedCicleDuringLastUpdate() )
		{
			if ( m_state == eGuiPageState::Hiding )
				m_state = eGuiPageState::Invisible;
			else if ( m_state == eGuiPageState::Showing )
				m_state  = eGuiPageState::Visible;

			for ( auto it = m_components.GetIterator(); it; it++ )
			{
				if ( m_state == eGuiPageState::Invisible )
					(*it)->OnHide();
				else
					(*it)->OnShow();
			}
		}
	}
	
	for ( auto it = m_components.GetIterator(); it; it++ )
	{
		(*it)->Update( this );
	}
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiPage::Render(const math::CameraTransform& camera)
{
	if ( m_state == eGuiPageState::Invisible )
		return;

	for ( auto it = m_components.GetIterator(); it; it++ )
	{
		(*it)->Render();
	}
}
//-------------------------------------------------------------------
//	Show
//-------------------------------------------------------------------
void GuiPage::Show(bool show)
{
	bool startTransition = false;

	if ( show && m_state != eGuiPageState::Visible && m_state != eGuiPageState::Showing )
	{
		startTransition = true;
		m_state = eGuiPageState::Showing;
	}
	else if ( !show && m_state != eGuiPageState::Invisible && m_state != eGuiPageState::Hiding )
	{
		startTransition = true;
		m_state = eGuiPageState::Hiding;
	}

	if ( startTransition )
	{
		m_componentInUse = HashString();

		if ( m_animationTime.HasCompletedCicleDuringLastUpdate() )
			m_animationTime.Reset();
		else
			m_animationTime.Break();
	}
}
//-------------------------------------------------------------------
//	Find
//-------------------------------------------------------------------
IGuiComponent* GuiPage::Find(const HashString& componentName)
{
	SmartPtr<IGuiComponent> pResult = m_components.Get( componentName.GetHash() );
	
	return pResult.Get();
}
//-------------------------------------------------------------------
const IGuiComponent* GuiPage::Find(const HashString& componentName)const
{
	const SmartPtr<IGuiComponent> pResult = m_components.Get( componentName.GetHash() );

	return pResult.Get();
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void GuiPage::Remove(const HashString& componentName)
{
	List<HashString> childrenHash;

	auto pComponent = Find( componentName );

	if ( pComponent )
	{
		for ( auto itChildren = pComponent->GetAllChildren().GetIterator(); itChildren; itChildren++ )
		{
			childrenHash.AddLast( (*itChildren)->GetName() );
		}

		pComponent->RemoveAllChildren();
		m_components.Remove( pComponent->GetName().GetHash() );
	}

	for ( auto it = childrenHash.GetIterator(); it; it++ )
		Remove( (*it) );
}
//-------------------------------------------------------------------
//	IsCursorOnTop
//-------------------------------------------------------------------
bool GuiPage::IsCursorOnTop()const
{
	if ( m_state != eGuiPageState::Invisible )
	{
		for ( auto it = m_components.GetIterator(); it; it++ )
		{
			if ( (*it)->IsPointOnTop(g_pInput->GetMousePos()) )
				return true;
		}
	}

	return false;
}
//-------------------------------------------------------------------
//	SetComponentInUse
//-------------------------------------------------------------------
void GuiPage::SetComponentInUse(IGuiComponent* pComponent)
{
	if ( !pComponent )
		m_componentInUse = HashString();
	else
	{
		CHECK( !m_componentInUse.IsValid() || m_componentInUse == pComponent->GetName() );

		m_componentInUse = pComponent->GetName();
	}

	if ( !Find(m_componentInUse) )
		m_componentInUse = HashString();
}


