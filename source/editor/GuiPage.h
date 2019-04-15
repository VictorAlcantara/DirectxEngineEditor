#pragma once

#include "../engine/Table.h"
#include "GuiUtil.h"
#include "../engine/Pointer.h"
#include "../engine/String.h"
#include "../engine/Math.h"
#include "../engine/Gradient.h"
#include "../engine/Exception.h"
#include "GuiComponent.h"
#include "../engine/Debug.h"

class ITexture;

/****************************************************************************************
	GuiPageBridge
*****************************************************************************************/

class IGuiPageBridge
{
	public:
		virtual void SetComponentInUse(IGuiComponent*) = 0;
		virtual float GetStateProgress()const = 0;
		virtual eGuiPageState GetState()const = 0;
		virtual const HashString& GetComponentInUse()const = 0;
};

/****************************************************************************************
	GuiPage
*****************************************************************************************/

class GuiPage : public IGuiPageBridge
{
	public:
		GuiPage(const HashString&);
		~GuiPage();

		void Update();
		void Render(const math::CameraTransform&);

		void Show(bool);

		template <class GUI_COMPONENT_TYPE>
		GUI_COMPONENT_TYPE* Add(const HashString& componentName, const HashString& parentName = "");

		template <class GUI_COMPONENT_TYPE>
		GUI_COMPONENT_TYPE* FindAs(const HashString& componentName);

		template <class GUI_COMPONENT_TYPE>
		const GUI_COMPONENT_TYPE* FindAs(const HashString& componentName)const;

		IGuiComponent* Find(const HashString& componentName);
		const IGuiComponent* Find(const HashString& componentName)const;

		void Remove(const HashString& componentName);

		eGuiPageState GetState()const override final { return m_state; }
		float GetStateProgress()const override final{ return math::clamp(m_animationTime.GetProgress(), 0.0f, 1.0f); }
		const HashString& GetName()const { return m_name; }

		bool IsCursorOnTop()const;

		void SetComponentInUse(IGuiComponent*)override final;

		const HashString& GetComponentInUse()const override final { return m_componentInUse; }

	private:
		HashString m_name;
		HashString m_componentInUse;
		eGuiPageState m_state;

		Table<SmartPtr<IGuiComponent>, uint> m_components;

		Gradient<float, GradientTransition_Linear, GradientPeriod_Repeat> m_animationTime;
};

/****************************************************************************************
	GuiPage template methods
*****************************************************************************************/

//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
template <class GUI_COMPONENT_TYPE>
inline GUI_COMPONENT_TYPE* GuiPage::Add(const HashString& componentName, const HashString& parentName)
{
		if ( !componentName.IsValid()  )
		{
			ASSERT( "Gui component must have a name" );
			return nullptr;
		}
	
		if ( Find(componentName) )
		{
			THROW( "GuiPage::Add() Gui component with same name already exists" );
		}
		else
		{
			GUI_COMPONENT_TYPE* pObject = new GUI_COMPONENT_TYPE();

			pObject->Initialize( componentName );

			m_components.Add( SmartPtr<IGuiComponent>(pObject), componentName.GetHash() );
	
			if ( parentName.IsValid() )
			{
				IGuiComponent* pParent = Find( parentName );

				if ( pParent )
				{
					pParent->AddChild( pObject );
				}
				else
				{
					ASSERT( "Parent not found" );
				}
			}
	
			return pObject;
		}
	
		return nullptr;
}
//-------------------------------------------------------------------
//	FindAs
//-------------------------------------------------------------------
template <class GUI_COMPONENT_TYPE>
inline GUI_COMPONENT_TYPE* GuiPage::FindAs(const HashString& componentName)
{
	SmartPtr<IGuiComponent> pResult = m_components.Get( componentName.GetHash() );

	if ( pResult && pResult->GetType() != GUI_COMPONENT_TYPE::GetTypeStatic() )
	{
		ASSERT( "Incorrect type" );
		pResult = nullptr;
	}
	
	return (GUI_COMPONENT_TYPE*)pResult.Get();
}
//-------------------------------------------------------------------
template <class GUI_COMPONENT_TYPE>
inline const GUI_COMPONENT_TYPE* GuiPage::FindAs(const HashString& componentName)const
{
	const SmartPtr<IGuiComponent> pResult = m_components.Get( componentName.GetHash() );

	if ( !pResult )
	{
		ASSERT( "Component not found" );
	}
	else if ( pResult->GetType() != GUI_COMPONENT_TYPE::GetTypeStatic() )
	{
		ASSERT( "Incorrect type" );
		pResult = nullptr;
	}

	return (const GUI_COMPONENT_TYPE*)pResult.Get();
}
