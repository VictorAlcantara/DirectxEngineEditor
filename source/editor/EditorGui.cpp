#include "EditorGui.h"
#include "../engine/Texture.h"
#include "../engine/TableHashFunction.h"
#include "GuiPage.h"

/****************************************************************************************
	List find function
*****************************************************************************************/

bool listFindFunc_GuiPage(GuiPage*const& pPage, const HashString& pageName)
{
	return pPage->GetName() == pageName;
}

/****************************************************************************************
	EditorGui class
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorGui::EditorGui() : m_pages( 50, tableHashFunc_Uint )
{
	m_initialized = false;
}
//-------------------------------------------------------------------
EditorGui::~EditorGui()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EditorGui::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_pageStack.Clear();
	m_pages.Clear();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EditorGui::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}
	
	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorGui::Update()
{
	for ( auto it = m_pages.GetIterator(); it; it++ )
	{
		(*it)->Update();
	}
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void EditorGui::Render(const math::CameraTransform& camera)
{
	for ( auto it = m_pages.GetIterator(); it; it++ )
	{
		(*it)->Render( camera );
	}
}
//-------------------------------------------------------------------
//	CreatePage
//-------------------------------------------------------------------
GuiPage* EditorGui::CreatePage(const HashString& pageName)
{
	GuiPage* pPage = FindPage( pageName );

	CHECK( !pPage );

	if ( !pPage )
	{
		pPage = new GuiPage(pageName);
		m_pages.Add( SmartPtr<GuiPage>(pPage), pageName.GetHash() );
	}

	return pPage;
}
//-------------------------------------------------------------------
//	HideAll
//-------------------------------------------------------------------
void EditorGui::HideAll()
{
	for ( auto it = m_pages.GetIterator(); it; it++ )
		(*it)->Show( false );
}
//-------------------------------------------------------------------
//	RemovePage
//-------------------------------------------------------------------
void EditorGui::RemovePage(const HashString& pageName)
{
	m_pages.Remove( pageName.GetHash() );
}
//-------------------------------------------------------------------
//	FindPage
//-------------------------------------------------------------------
GuiPage* EditorGui::FindPage(const HashString& pageName)
{
	SmartPtr<GuiPage> pResult = m_pages.Get( pageName.GetHash() );
	return pResult.Get();
}
//-------------------------------------------------------------------
const GuiPage* EditorGui::FindPage(const HashString& pageName)const
{
	const SmartPtr<GuiPage> pResult = m_pages.Get( pageName.GetHash() );
	return pResult.Get();
}
//-------------------------------------------------------------------
//	RemoveFromAll
//-------------------------------------------------------------------
void EditorGui::RemoveFromAll(const HashString& pageName)
{
	for ( auto it = m_pages.GetIterator(); it; it++ )
	{
		(*it)->Remove( pageName );
		RemoveFromStack( pageName );
	}
}
//-------------------------------------------------------------------
//	AddToStack
//-------------------------------------------------------------------
void EditorGui::AddToStack(GuiPage* pPage)
{
	if ( pPage )
	{
		RemoveFromStack( pPage->GetName() );
		m_pageStack.AddLast( pPage );
	}
}
//-------------------------------------------------------------------
//	RemoveStackTop
//-------------------------------------------------------------------
void EditorGui::RemoveStackTop()
{
	auto pNode = m_pageStack.GetLast();

	if ( pNode )
		pNode->Remove();
}
//-------------------------------------------------------------------
//	RemoveStackBottom
//-------------------------------------------------------------------
void EditorGui::RemoveStackBottom()
{
	auto pNode = m_pageStack.GetFirst();

	if ( pNode )
		pNode->Remove();
}
//-------------------------------------------------------------------
//	RemoveFromStack
//-------------------------------------------------------------------
void EditorGui::RemoveFromStack(const HashString& pageName)
{
	m_pageStack.Remove( pageName, listFindFunc_GuiPage );
}
//-------------------------------------------------------------------
//	ClearStack
//-------------------------------------------------------------------
void EditorGui::ClearStack()
{
	m_pageStack.Clear();
}
//-------------------------------------------------------------------
//	GetStackCount
//-------------------------------------------------------------------
uint EditorGui::GetStackCount()const
{
	return m_pageStack.Count();
}
//-------------------------------------------------------------------
//	GetStackTop
//-------------------------------------------------------------------
GuiPage* EditorGui::GetStackTop()
{
	auto pNode = m_pageStack.GetLast();

	if ( pNode )
		return pNode->GetValue();

	return nullptr;
}
//-------------------------------------------------------------------
const GuiPage* EditorGui::GetStackTop()const
{
	auto pNode = m_pageStack.GetLast();

	if ( pNode )
		return pNode->GetValue();

	return nullptr;
}
//-------------------------------------------------------------------
//	GetStackBottom
//-------------------------------------------------------------------
GuiPage* EditorGui::GetStackBottom()
{
	auto pNode = m_pageStack.GetFirst();

	if ( pNode )
		return pNode->GetValue();

	return nullptr;
}
//-------------------------------------------------------------------
const GuiPage* EditorGui::GetStackBottom()const
{
	auto pNode = m_pageStack.GetFirst();

	if ( pNode )
		return pNode->GetValue();

	return nullptr;
}
//-------------------------------------------------------------------
//	GetStack
//-------------------------------------------------------------------
const List<GuiPage*>& EditorGui::GetStack()const
{
	return m_pageStack;
}
//-------------------------------------------------------------------
//	IsCursorOnTopAnyPage
//-------------------------------------------------------------------
bool EditorGui::IsCursorOnTopAnyPage()const
{
	for ( auto it = m_pages.GetIterator(); it; it++ )
	{
		if ( (*it)->IsCursorOnTop() )
			return true;
	}

	return false;
}
