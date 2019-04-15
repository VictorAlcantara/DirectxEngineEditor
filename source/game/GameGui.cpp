#include "GameGui.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../engine/Effect.h"
#include "../engine/FontEffect.h"
#include "../engine/Font.h"
#include "../engine/Entity.h"
#include "Entity_Player.h"
#include "GameGlobal.h"

/****************************************************************************************
	IGameGuiMode
*****************************************************************************************/

uint IGameGuiMode::s_idControl = 0;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
IGameGuiMode::IGameGuiMode()
{
	m_pOwner = nullptr;
	m_id = ++s_idControl;
}

/****************************************************************************************
	GameGuiMode_Message
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameGuiMode_Message::GameGuiMode_Message()
{
	m_wps = 33.0f;
	m_duration = 5.0f;
	m_flag = 0;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GameGuiMode_Message::OnShutdown()
{
	m_pEffectColor->Shutdown();
	m_pEffectColor = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GameGuiMode_Message::OnInitialize()
{
	m_pEffectColor = new Effect_Color;
	m_pEffectColor->Initialize();
	m_pEffectColor->GetConfig().color4 = math::normalizeRgb( 0, 140 );
	m_pEffectColor->GetConfig().renderer.blendMode = eBlendMode::Alpha;
	m_pEffectColor->GetConfig().renderer.depthOperation = eDepthOperation::Always;

	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetColor( math::normalizeRgb(204) );
	m_pFontEffect->SetCharSpaceProportion( 0.05f );
	m_pFontEffect->SetCharSize( 24 );

	m_hModel = primitive::geometry::createSquare();
}
//-------------------------------------------------------------------
//	Interrupt
//-------------------------------------------------------------------
void GameGuiMode_Message::Interrupt()
{
	m_pOwner->ChangeMode( eGuiMode::None );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GameGuiMode_Message::Update()
{
	m_chronometer.Update( g_pCore->GetTimeSinceLastFrame() );

	if ( m_chronometer.GetTime() > m_duration )
		Interrupt();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GameGuiMode_Message::Render()
{
	float z = 0.1f;
	float fontSize = (float)m_pFontEffect->GetCharSize();
	math::Vec2 textArea = m_hFont->GetTextArea( m_text, fontSize, m_pFontEffect->GetCharSpaceProportion(), m_pFontEffect->GetLineBreakProportion() );
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 viewportScale = viewportDimension * 0.5f;
	math::Vec3 boxPos;
	math::Vec3 boxScale( viewportScale.x * 0.85f, textArea.y * 0.5f + fontSize * 0.5f, 1.0f );

	// Box
	{
		boxPos = math::Vec3( 0.0f, viewportScale.y * -0.85f + boxScale.y, z );
		
		math::Mtx44 mWorld = math::matrixScale( boxScale ) * math::matrixTranslate( boxPos );
		math::Mtx44 mView;
		math::Mtx44 mProj = math::matrixOrtho( viewportDimension );

		m_pEffectColor->Render( m_hModel, mWorld, mView, mProj );
	}

	// Text
	{
		uint wordsToShow = (uint)( m_wps * m_chronometer.GetTime() );

		math::Vec2 boxPosScreen = math::centerToScreenCoord( boxPos.Xy(), viewportDimension );
		math::Vec2 posScreen = boxPosScreen - boxScale.Xy() + math::Vec2(20.0f, 3.0f);

		m_pFontEffect->SetScreenPos( posScreen );
		m_pFontEffect->Render( string::copy( m_text, 0, wordsToShow ), m_hFont.GetPtr() );
	}
}
//-------------------------------------------------------------------
//	SetText
//-------------------------------------------------------------------
void GameGuiMode_Message::SetText(const String& text)
{
	m_text = text;
	m_chronometer.Reset();
	m_duration = ((float)m_text.Length() / m_wps) * 2.8f;
}

/****************************************************************************************
	GameGuiMode_FullScreenMessage
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameGuiMode_FullScreenMessage::GameGuiMode_FullScreenMessage()
{
	m_wps = 33.0f;
	m_duration = 5.0f;
	m_canInterrupt = true;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::OnShutdown()
{
	m_pEffectColor->Shutdown();
	m_pEffectColor = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;

	if ( m_cmdEnd.Length() > 0 )
		g_pCommand->Execute( m_cmdEnd );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::OnInitialize()
{
	m_pEffectColor = new Effect_Color;
	m_pEffectColor->Initialize();
	m_pEffectColor->GetConfig().color4 = math::normalizeRgb( 0, 255 );
	m_pEffectColor->GetConfig().renderer.blendMode = eBlendMode::Disabled;
	m_pEffectColor->GetConfig().renderer.depthOperation = eDepthOperation::Always;

	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetColor( math::normalizeRgb(204) );
	m_pFontEffect->SetCharSpaceProportion( 0.05f );
	m_pFontEffect->SetCharSize( 40 );

	m_hModel = primitive::geometry::createSquare();
}
//-------------------------------------------------------------------
//	Interrupt
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::Interrupt()
{
	m_pOwner->ChangeMode( eGuiMode::None );
}
//-------------------------------------------------------------------
//	OnEnd
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::OnEnd(const String& cmd)
{
	m_cmdEnd = cmd;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::Update()
{
	m_chronometer.Update( g_pCore->GetTimeSinceLastFrame() );

	if ( m_chronometer.GetTime() > m_duration && m_canInterrupt )
		Interrupt();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::Render()
{
	float z = 0.1f;
	float fontSize = (float)m_pFontEffect->GetCharSize();
	math::Vec2 textArea = m_hFont->GetTextArea( m_text, fontSize, m_pFontEffect->GetCharSpaceProportion(), m_pFontEffect->GetLineBreakProportion() );
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 viewportScale = viewportDimension * 0.5f;
	math::Vec3 boxScale( viewportScale.x, viewportScale.y, 1.0f );

	// Box
	{
		math::Mtx44 mWorld = math::matrixScale( boxScale ) * math::matrixTranslate( 0.0f, 0.0f, z );
		math::Mtx44 mView;
		math::Mtx44 mProj = math::matrixOrtho( viewportDimension );

		m_pEffectColor->Render( m_hModel, mWorld, mView, mProj );
	}

	// Text
	{
		uint wordsToShow = (uint)( m_wps * m_chronometer.GetTime() );

		math::Vec2 posScreen( viewportDimension.x * 0.2f, viewportDimension.y - fontSize * 4.0f );

		m_pFontEffect->SetScreenPos( posScreen );
		m_pFontEffect->SetZ( z * 0.99f );
		m_pFontEffect->Render( string::copy( m_text, 0, wordsToShow ), m_hFont.GetPtr() );
	}
}
//-------------------------------------------------------------------
//	SetText
//-------------------------------------------------------------------
void GameGuiMode_FullScreenMessage::SetText(const String& text)
{
	m_text = text;
	m_chronometer.Reset();
	m_duration = ((float)m_text.Length() / m_wps) * 1.2f;
}

/****************************************************************************************
	GameGui
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameGui::GameGui()
{
}
//-------------------------------------------------------------------
GameGui::~GameGui()
{
}
//-------------------------------------------------------------------
//	OnExit
//-------------------------------------------------------------------
void GameGui::OnExit()
{
	if ( m_pMode )
		m_pMode->Shutdown();
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void GameGui::OnEnter()
{
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GameGui::Update()
{
	if ( m_pMode )
		m_pMode->Update();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GameGui::Render()
{
	if ( m_pMode )
		m_pMode->Render();
}
//-------------------------------------------------------------------
//	ChangeMode
//-------------------------------------------------------------------
IGameGuiMode* GameGui::ChangeMode(eGuiMode mode)
{
	if ( m_pMode )
		m_pMode->Shutdown();

	switch ( mode )
	{
		case eGuiMode::None:
			m_pMode = nullptr;
			break;
		case eGuiMode::Message:
			m_pMode = new GameGuiMode_Message;
			break;
		case eGuiMode::FullScreenMessage:
			m_pMode = new GameGuiMode_FullScreenMessage;
			break;
		default:
			ASSERT( "Uncategorized gui mode" );
	};

	if ( m_pMode )
	{
		m_pMode->SetOwner( this );
		m_pMode->Initialize();
	}

	return m_pMode.Get();
}

/****************************************************************************************
	Process_GuiDisplayRadiusMonitor
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_GuiDisplayRadiusMonitor::Process_GuiDisplayRadiusMonitor()
{
	m_radius = 3.0f;
	m_idGuiMode = 0;
}
//-------------------------------------------------------------------
//	SetTarget
//-------------------------------------------------------------------
void Process_GuiDisplayRadiusMonitor::SetTarget(Handle<Entity> hEntity, IGameGuiMode* pGuiMode)
{
	m_idGuiMode = pGuiMode->GetId();
	m_hTarget = hEntity;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_GuiDisplayRadiusMonitor::Update()
{
	if ( m_hTarget )
	{
		IGameGuiMode* pGuiMode = m_pGui->GetMode();

		if ( pGuiMode && pGuiMode->GetId() == m_idGuiMode )
		{
			CHECK( m_hTarget->GetClassRelationship_Is<Entity_Player>() );

			if ( math::distSq(entityGet_pos(m_hTarget), m_lockPos) > math::sq(m_radius) )
			pGuiMode->Interrupt();
		}
	}
}
