#include "GuiComponent.h"
#include "../engine/Debug.h"
#include "../engine/Effect.h"
#include "../engine/Vertex.h"
#include "../engine/Model.h"
#include "../engine/CoreInterface.h"
#include "EditorGlobal.h"
#include "../engine/RenderTarget.h"
#include "../engine/Texture.h"
#include "EditorGui.h"
#include "../engine/Random.h"
#include "../engine/Primitive.h"
#include "GuiAnimation.h"
#include "GuiPage.h"
#include "../engine/Font.h"
#include "../engine/FontEffect.h"
#include "../shared/ProcessShared.h"

/****************************************************************************************
	Helper functions
*****************************************************************************************/

//-------------------------------------------------------------------
//	listFindFunc_Component
//-------------------------------------------------------------------
bool isCharValid(char c, uint allowedFlag)
{
	if ( (allowedFlag & GUI_TEXT_ALLOW_LETTER) && (math::between( c, 'a', 'z' ) || math::between( c, 'A', 'Z' )) )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_NUMBER) && math::between( c, '0', '9' ) )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_SPACE) && c == ' '  )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_UNDERSCORE) && (c == '_') )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_QUOTE) && (c == '\'' || c == '\"') )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_COMMA) && (c == ',') )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_PERIOD) && (c == '.') )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_MARK) && string::contains("!?", c)  )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_BRACKET) && string::contains("(){}[]", c)  )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_COLON) && string::contains(":;", c)  )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_NEGATIVE) && string::contains("-", c)  )
		return true;
	if ( (allowedFlag & GUI_TEXT_ALLOW_MATH_SYMBOL) && string::contains("+-*/=%", c)  )
		return true;

	return false;
}
//-------------------------------------------------------------------
//	listFindFunc_Component
//-------------------------------------------------------------------
bool listFindFunc_Component(IGuiComponent* const& pComponent, const HashString& name)
{
	return pComponent->GetName() == name;
}
//-------------------------------------------------------------------
//	getAlphaForState
//-------------------------------------------------------------------
float getAlphaForState(const GuiPageInfo& pageInfo)
{
	float result = 1.0f;
	float progress = pageInfo.transitionProgress;

	if ( pageInfo.state == eGuiPageState::Invisible )
		progress = 0.0f;
	else if ( pageInfo.state == eGuiPageState::Hiding )
		progress = 1.0f - progress;
	else if ( pageInfo.state == eGuiPageState::Showing )
		progress = progress;
	else if ( pageInfo.state == eGuiPageState::Visible )
		progress = 1.0f;

	result = math::pow( progress, 5 );

	return result;
}

/****************************************************************************************
	GuiComponent_Base
*****************************************************************************************/

uint GuiComponent_Base::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Base::GuiComponent_Base(GuiConfig_Base* pConfigBase, float posScaleLeft, float posScaleTop, float posScaleCenterHorizontal, float posScaleCenterVertical, float posScaleRight, float posScaleBottom)
{
	CHECK( pConfigBase );

	m_id = s_idControl++;
	m_pParent = nullptr;
	m_posScaleLeft = posScaleLeft;
	m_posScaleTop = posScaleTop;
	m_posScaleCenterHorizontal = posScaleCenterHorizontal;
	m_posScaleCenterVertical = posScaleCenterVertical;
	m_posScaleRight = posScaleRight;
	m_posScaleBottom = posScaleBottom;
	m_pConfigBase = pConfigBase;

	m_initialized = false;
}
//-------------------------------------------------------------------
GuiComponent_Base::GuiComponent_Base(GuiConfig_Base* pConfigBase, float posScaleLeftTop, float posScaleCenter, float posScaleRightBottom) :
	GuiComponent_Base( pConfigBase, posScaleLeftTop, posScaleLeftTop, posScaleCenter, posScaleCenter, posScaleRightBottom, posScaleRightBottom )
{
}
//-------------------------------------------------------------------
GuiComponent_Base::~GuiComponent_Base()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void GuiComponent_Base::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	OnShutdown();
	RemoveAllChildren();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void GuiComponent_Base::Initialize(const HashString& name)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	if ( m_name.IsValid() )
	{
		ASSERT( "Gui component already initialized" );
		return;
	}

	m_name = name;

	OnInitialize();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GuiComponent_Base::Update(IGuiPageBridge* pPageBridge)
{
	m_pageInfo.state = pPageBridge->GetState();
	m_pageInfo.transitionProgress = pPageBridge->GetStateProgress();

	if ( m_pageInfo.state == eGuiPageState::Visible )
		OnUpdate( pPageBridge );
}
//-------------------------------------------------------------------
//	GetAllChildren
//-------------------------------------------------------------------
const List<IGuiComponent*>& GuiComponent_Base::GetAllChildren()const
{
	return m_children;
}
//-------------------------------------------------------------------
//	AddChild
//-------------------------------------------------------------------
void GuiComponent_Base::AddChild(IGuiComponent* pChild)
{
	if ( m_children.Get( pChild->GetName(), listFindFunc_Component ) )
	{
		ASSERT( "Child already inserted" );
	}
	else
	{
		m_children.AddLast( pChild );
		pChild->SetParent( this );
	}
}
//-------------------------------------------------------------------
//	RemoveChild
//-------------------------------------------------------------------
void GuiComponent_Base::RemoveChild(const HashString& name)
{
	for ( auto pNode = m_children.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue()->GetName() == name )
		{
			pNode->GetValue()->SetParent( nullptr );
			pNode->Remove();

			return;
		}
	}
}
//-------------------------------------------------------------------
//	RemoveAllChildren
//-------------------------------------------------------------------
void GuiComponent_Base::RemoveAllChildren()
{
	for ( auto it = m_children.GetIterator(); it; it++ )
	{
		(*it)->SetParent( nullptr );
	}

	m_children.Clear();
}
//-------------------------------------------------------------------
//	SetParent
//-------------------------------------------------------------------
void GuiComponent_Base::SetParent(IGuiComponent* pComponent)
{
	m_pParent = pComponent;
}
//-------------------------------------------------------------------
//	IsPointOnQuad
//-------------------------------------------------------------------
bool GuiComponent_Base::IsPointOnTop(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();

	return math::pointOnQuad( pointScreen, math::Aabb2(pos - scale, pos + scale) );
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Base::GetPos()const
{
	math::Vec2 result;
	math::ViewportInfo viewportParent = g_pRenderer->GetWindowViewport();
	math::Vec2 scale = GetScale();
	auto configBase = GetConfigBase();

	if ( m_pParent )
	{
		math::Vec2 scaleParent = m_pParent->GetScale();
		math::Vec2 posParent = m_pParent->GetPos();
		viewportParent.width = (uint)(scaleParent.x * 2.0f);
		viewportParent.height = (uint)(scaleParent.y * 2.0f);
		viewportParent.left = (uint)(posParent.x - scaleParent.x);
		viewportParent.top = (uint)(posParent.y - scaleParent.y);
	}

	switch ( configBase.anchor.horizontal )
	{
		case eGuiAnchorHorizontal::Left:
			result.x = viewportParent.width * 0.0f + configBase.anchor.paddingHorizontal + scale.x * m_posScaleLeft;
			break;
		case eGuiAnchorHorizontal::Center:
			result.x = viewportParent.width * 0.5f + configBase.anchor.paddingHorizontal + scale.x * m_posScaleCenterHorizontal;
			break;
		case eGuiAnchorHorizontal::Right:
			result.x = viewportParent.width * 1.0f - configBase.anchor.paddingHorizontal + scale.x * m_posScaleRight;
			break;
	}

	switch ( GetConfigBase().anchor.vertical )
	{
		case eGuiAnchorVertical::Top:
			result.y = viewportParent.height * 0.0f + configBase.anchor.paddingVertical + scale.y * m_posScaleTop;
			break;
		case eGuiAnchorVertical::Center:
			result.y = viewportParent.height * 0.5f + configBase.anchor.paddingVertical + scale.y * m_posScaleCenterVertical;
			break;
		case eGuiAnchorVertical::Bottom:
			result.y = viewportParent.height * 1.0f - configBase.anchor.paddingVertical + scale.y * m_posScaleBottom;
			break;
	}

	result.x += viewportParent.left;
	result.y += viewportParent.top;

	return result;
}
//-------------------------------------------------------------------
//	SetInUse
//-------------------------------------------------------------------
void GuiComponent_Base::SetInUse(IGuiPageBridge* pPageBridge, bool inUse)
{
	if ( !pPageBridge->GetComponentInUse().IsValid() || pPageBridge->GetComponentInUse() == GetName() )
		pPageBridge->SetComponentInUse( inUse ? this : nullptr );
}
//-------------------------------------------------------------------
//	CanUse
//-------------------------------------------------------------------
bool GuiComponent_Base::CanUse(IGuiPageBridge* pPageBridge)const
{
	return !pPageBridge->GetComponentInUse().IsValid() || pPageBridge->GetComponentInUse() == GetName();
}
//-------------------------------------------------------------------
//	GetZ
//-------------------------------------------------------------------
float GuiComponent_Base::GetZ()const
{
	return (float)GetConfigBase().layer * 0.1f;
}
//-------------------------------------------------------------------
//	ValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Base::ValidateConfig()const
{
	#ifdef DEBUG_MODE
		CHECK( math::between<uint>(GetConfigBase().layer, 1, 9) );
		CHECK( m_name.IsValid() );

		OnValidateConfig();
	#endif
}
//-------------------------------------------------------------------
//	GetAnimationColor
//-------------------------------------------------------------------
math::Vec4 GuiComponent_Base::GetAnimationColor(const GuiAnimation* pAnimation, const math::Vec4& color)const
{
	math::Vec4 result = color;

	if ( pAnimation )
	{
		result.w *= pAnimation->Get( GetPageInfo().transitionProgress, GetPageInfo().state ).alpha;
		result = math::saturateRgb( result, pAnimation->Get( GetPageInfo().transitionProgress, GetPageInfo().state ).saturate );
	}
	else
	{
		result.w = getAlphaForState( GetPageInfo() );
	}

	return result;
}
//-------------------------------------------------------------------
//	GetAnimationTransform
//-------------------------------------------------------------------
math::Mtx44 GuiComponent_Base::GetAnimationTransform(const GuiAnimation* pAnimation)const
{
	math::Mtx44 result;

	if ( pAnimation )
	{
		math::Vec2 scale = pAnimation->Get( GetPageInfo().transitionProgress, GetPageInfo().state ).scale;
		result = math::matrixScale( scale );
	}

	return result;
}

/****************************************************************************************
	GuiComponent_Panel
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Panel::GuiComponent_Panel() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Panel::OnShutdown()
{
	m_pRenderTarget->Shutdown();
	m_pRenderTarget = nullptr;

	m_pEffectTexture->Shutdown();
	m_pEffectTexture = nullptr;

	m_pEffectBlur->Shutdown();
	m_pEffectBlur = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Panel::OnInitialize()
{
	m_hModel = primitive::geometry::createSquare();

	float weights[] = { 
		100,  90,  80,  75, 
		54,  53,  52,  41,
		34,  13,  12,  11,
		4,  3,  2,  1,
	};

	m_pEffectBlur = new Effect_Blur;
	m_pEffectBlur->Initialize();
	m_pEffectBlur->SetRenderTargetDimension( g_pRenderer->GetWindowViewport().GetDimension() );
	m_pEffectBlur->GetConfig().displace = 1.0f;
	m_pEffectBlur->GetConfig().baseWeight = 0.0f;
	m_pEffectBlur->GetConfig().SetWeights( weights, ARRAY_COUNT(weights) );
	m_pEffectBlur->GetConfig().blurDirPass0 = math::normalize( math::Vec2(1.0f, 0.0f) );
	m_pEffectBlur->GetConfig().blurDirPass1 = math::normalize( math::Vec2(0.0f, 1.0f) );

	m_pEffectTexture = new Effect_Texture;
	m_pEffectTexture->Initialize();
	m_pEffectTexture->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pRenderTarget = new RenderTarget;
	m_pRenderTarget->Initialize( g_pRenderer->GetWindowViewport().GetDimension(), eColorFormat::Rgba_32_Float );
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Panel::Render()
{
	math::Mtx33 textureTransform = GetTextureTransform();
	math::Vec2 pos = GetPos();
	math::Vec3 scale = GetScale().Xyz(1.0f);
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();

	pos = math::screenCoordToCenter( pos, viewportDimension );

	math::Mtx44 world = math::matrixScale( scale ) * math::matrixTranslate( pos.Xyz(GetZ()) );
	math::Mtx44 view;
	math::Mtx44 proj = math::matrixOrtho( viewportDimension );

	m_pEffectBlur->GetConfig().hTexture = g_pScene->GetSceneTexture();
	m_pEffectBlur->GetConfig().textureTransform = textureTransform;
	m_pRenderTarget->Clear( 0.2f, 0.5f, 0.1f, 1.0f );
	m_pEffectBlur->Render( m_pRenderTarget.Get(), m_hModel, world, view, proj );

	m_pEffectTexture->GetConfig().color4 = GetAnimationColor( m_config.pAnimation.Get(), m_config.color4 );
	m_pEffectTexture->GetConfig().hTexture = m_pRenderTarget->GetTexture();
	m_pEffectTexture->GetConfig().textureTransform = math::Mtx33();
	m_pEffectTexture->Render( m_hModel, GetAnimationTransform( m_config.pAnimation.Get() ) * world, view, proj );
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Panel::GetScale()const
{
	math::Vec2 result = GetConfig().dimension;
	
	return result * 0.5f;
}
//-------------------------------------------------------------------
//	GetTextureTranform
//-------------------------------------------------------------------
math::Mtx33 GuiComponent_Panel::GetTextureTransform()const
{
	math::Mtx33 result;

	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 texTranslate = math::scaleInv( pos - scale, viewportDimension );
	math::Vec2 texScale = math::scaleInv( scale * 2.0f, viewportDimension );

	result = math::matrixScale( texScale ) * math::matrixTranslate( texTranslate );

	return result;
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Panel::OnValidateConfig()const
{
	CHECK( m_config.dimension.x > 0.0f );
	CHECK( m_config.dimension.y > 0.0f );
}
/****************************************************************************************
	GuiComponent_Button
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Button::GuiComponent_Button() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Button::OnShutdown()
{
	m_pEffectTexture->Shutdown();
	m_pEffectTexture = nullptr;

	m_pEffectModel->Shutdown();
	m_pEffectModel = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Button::OnInitialize()
{
	m_hButtonModel = primitive::geometry::createSquare();

	m_pEffectTexture = new Effect_Texture;
	m_pEffectTexture->Initialize();

	m_hTextureDefault = primitive::texture::createWhite();

	CHECK( m_hTextureDefault.IsValid() );

	m_pEffectTexture->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pEffectModel = new Effect_LightDir4;
	m_pEffectModel->Initialize();
	m_pEffectModel->GetConfig().ambientColor4 = math::normalizeRgb( 50 );
	m_pEffectModel->GetConfig().diffuseColor4 = math::normalizeRgb( 255 );
	m_pEffectModel->GetConfig().SetLightDir( 0, math::normalize(1.0f, -1.0f, 1.0f) );
	m_pEffectModel->GetConfig().SetLightColor( 0, math::normalizeRgb(255) );
	m_pEffectModel->GetConfig().SetLightDir( 1, math::normalize(0.0f, 1.0f, 0.0f) );
	m_pEffectModel->GetConfig().SetLightColor( 1, math::normalizeRgb(170) );
	m_pEffectModel->GetConfig().hTexture = primitive::texture::createWhite();
	m_pEffectModel->GetConfig().renderer.depthOperation = eDepthOperation::Always;
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_Button::OnUpdate(IGuiPageBridge* pPageBridge)
{
	if ( g_pInput->IsKeyPress(eKey::KEY_MOUSE_LEFT) && GetPageInfo().state == eGuiPageState::Visible )
	{
		if ( IsPointOnTop(g_pInput->GetMousePos()) )
			m_config.pClickFunction( this, m_config.flag );
	}

	if ( IsPointOnTop(g_pInput->GetMousePos()) )
		m_color = m_config.mouseOverColor4;
	else
		m_color = m_config.idleColor4;

	float dt = g_pCore->GetTimeSinceLastFrame();
	math::Euler modelVelocityConverted = m_config.modelAngularVelocity.As( m_modelRotation.type );
	m_modelRotation.yaw += modelVelocityConverted.yaw * dt;
	m_modelRotation.pitch += modelVelocityConverted.pitch * dt;
	m_modelRotation.roll += modelVelocityConverted.roll * dt;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Button::Render()
{
	math::Vec2 screenPos = GetPos();
	math::Vec3 pos = math::screenCoordToCenter( screenPos, g_pRenderer->GetWindowViewport().GetDimension() ).Xyz( GetZ() );
	math::Vec3 scale = GetScale().Xyz(1.0f);
	math::Vec4 color = GetAnimationColor( m_config.pAnimation.Get(), m_color );

	m_pEffectTexture->GetConfig().renderer.blendMode = GetConfig().allowTransparency ? eBlendMode::Alpha : eBlendMode::Disabled;

	{
		math::Mtx44 world = math::matrixScale( scale ) * math::matrixTranslate( pos );
		math::Mtx44 view;
		math::Mtx44 proj = math::matrixOrtho( g_pRenderer->GetWindowViewport().GetDimension() );

		m_pEffectTexture->GetConfig().color4 = color;
		m_pEffectTexture->GetConfig().hTexture = m_config.hTexture.IsValid() ? m_config.hTexture : m_hTextureDefault;
		m_pEffectTexture->Render( m_hButtonModel, GetAnimationTransform( m_config.pAnimation.Get() ) * world, view, proj );
	}

	if ( m_config.hModel )
	{
		math::ViewportInfo viewportPrev = g_pRenderer->GetWindowViewport();
		math::ViewportInfo viewport;
		viewport.left = (int)(screenPos.x - scale.x);
		viewport.top = (int)(screenPos.y - scale.y);
		viewport.width = (uint)(scale.x * 2.0f);
		viewport.height = (uint)(scale.y * 2.0f);

		g_pRenderer->SetViewport( viewport );

		float modelZ = 2.0f;
		math::Vec3 modelScale = m_config.hModel->GetAabb().GetScale();

		if ( !math::isZero(modelScale.x) ) modelScale.x = 1.0f / modelScale.x;
		if ( !math::isZero(modelScale.y) ) modelScale.y = 1.0f / modelScale.y;
		if ( !math::isZero(modelScale.z) ) modelScale.z = 1.0f / modelScale.z;

		modelScale.z *= 0.5f;

		math::Mtx44 world = math::matrixScale(math::Vec3(modelScale)) * math::matrixRotateEuler(m_modelRotation) * math::matrixTranslate( 0.0f, 0.0f, modelZ );
		math::Mtx44 view = math::matrixViewLookUp( math::Vec3(0.0f, 0.0f, 0.0f), math::AXIS_Z, math::AXIS_Y );
		math::Mtx44 proj = math::matrixPerspective( viewport.GetAspectRatio(), 70.0f, 0.001f, modelZ * 2.0f );

		math::Mtx44 animationTransform = GetAnimationTransform( m_config.pAnimation.Get() );
		animationTransform = animationTransform * animationTransform;

		m_pEffectModel->Render( m_config.hModel, animationTransform * world, view, proj );

		g_pRenderer->SetViewport( viewportPrev );
	}
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Button::GetScale()const
{
	math::Vec2 result = m_config.dimension* 0.5f;

	return result;
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Button::OnValidateConfig()const
{
	CHECK( m_config.pClickFunction );
}

/****************************************************************************************
	GuiComponent_Text
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Text::GuiComponent_Text() : GuiComponent_Base(&m_config, 0.0f, 0.0f, -1.0f, -2.0f, -2.0f, -2.0f)
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Text::OnShutdown()
{
	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Text::OnInitialize()
{
	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Text::Render()
{
	math::Vec2 pos = GetPos();

	math::Vec4 color = m_config.color4;
	color.w *= getAlphaForState( GetPageInfo() );
	m_pFontEffect->SetColor( color );
	m_pFontEffect->SetScreenPos( pos );
	m_pFontEffect->SetZ( GetZ() );
	m_pFontEffect->Render( m_config.text, &m_hFont.Get() );
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Text::GetScale()const
{
	CHECK( m_hFont.IsValid() );

	math::Vec2 result = m_hFont->GetLineDimension( m_config.text, (float)m_pFontEffect->GetCharSize(), (float)m_pFontEffect->GetCharSpaceProportion() );

	return result * 0.5f;
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Text::OnValidateConfig()const
{
	CHECK( m_config.charSize >= 0.0f );
	CHECK( m_config.charSpaceProportion >= 0.0f );
	CHECK( m_config.fontName.Length() > 0 );
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_Text::SetConfig(const GuiConfig_Text& config)
{
	m_config = config;

	ValidateConfig();

	m_hFont = g_pResource->GetFontMngr()->LoadFont( config.fontName );

	m_pFontEffect->SetCharSize( (uint)m_config.charSize );
	m_pFontEffect->SetCharSpaceProportion( m_config.charSpaceProportion );
}

/****************************************************************************************
	GuiComponent_Slider
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Slider::GuiComponent_Slider() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
	m_saturate = 0.8f;
	m_inUse = false;
	m_percentage = 0.0f;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Slider::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Slider::OnInitialize()
{
	m_hModelSlider = primitive::geometry::createSquare();
	m_hModelCircle = primitive::geometry::createCircle(16);

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_Slider::OnUpdate(IGuiPageBridge* pPageBridge)
{
	if ( g_pInput->IsKeyUp(KEY_MOUSE_LEFT) )
		m_inUse = false;

	if ( CanUse(pPageBridge) && IsPointOnTop(g_pInput->GetMousePos()) )
	{
		if ( g_pInput->IsKeyPress(KEY_MOUSE_LEFT) || g_pInput->IsKeyDown(KEY_MOUSE_LEFT) )
			m_inUse = true;
		else if ( g_pInput->IsKeyPress(KEY_MOUSE_RIGHT) )
		{
			if ( GetConfig().defaultValue.use )
				SetValue( GetConfig().defaultValue.value );
		}
	}

	if ( m_inUse )
	{
		math::Vec2 pos = GetPos();
		math::Vec2 scale = GetScale();
		float xMouse = g_pInput->GetMousePos().x;
		float xPosMin = pos.x - scale.x;
		float xPosMax = pos.x + scale.x;
		float percentage = (xMouse - xPosMin) / (xPosMax - xPosMin);

		percentage = math::clamp( percentage, 0.0f, 1.0f );

		SetPercentage( percentage );
	}

	SetInUse( pPageBridge, m_inUse );

	UpdateSaturation(pPageBridge);
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Slider::OnValidateConfig()const
{
}
//-------------------------------------------------------------------
//	UpdateSaturation
//-------------------------------------------------------------------
void GuiComponent_Slider::UpdateSaturation(IGuiPageBridge* pPageBridge)
{
	m_saturate = 0.8f;

	if ( m_inUse )
		m_saturate = 1.0f;
	else if ( CanUse(pPageBridge) && IsPointOnTopCircle( g_pInput->GetMousePos() ) )
		m_saturate = 0.9f;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Slider::Render()
{
	float z = GetZ();
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Mtx44 proj = math::matrixOrtho( viewportDimension );

	// Slider rendering
	{
		math::Vec3 sliderPos = math::screenCoordToCenter(GetPos(), viewportDimension).Xyz(z);
		math::Vec3 sliderScale = GetScale().Xyz(1.0f);
		
		math::Mtx44 world = math::matrixScale( sliderScale ) * math::matrixTranslate( sliderPos );
		math::Mtx44 view;

		m_pEffect->GetConfig().color4 = math::Vec4(m_config.sliderColor3, getAlphaForState( GetPageInfo() ));
		m_pEffect->Render( m_hModelSlider, world, view, proj );
	}

	// Circle rendering
	{
		math::Vec3 circlePos = math::screenCoordToCenter(GetCirclePos(), viewportDimension).Xyz(z * 0.99f);
		math::Vec3 circleScale = GetCircleScale().Xyz(1.0f);

		math::Mtx44 world = math::matrixScale( circleScale ) * math::matrixTranslate( circlePos );
		math::Mtx44 view;

		m_pEffect->GetConfig().color4 = math::Vec4(m_config.circleColor3 * m_saturate, getAlphaForState( GetPageInfo() ) );
		m_pEffect->Render( m_hModelCircle, world, view, proj );
	}
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
void GuiComponent_Slider::SetValue(float value)
{
	float percentage = (value - m_config.minValue) / (m_config.maxValue - m_config.minValue);

	SetPercentage( percentage );
}
//-------------------------------------------------------------------
//	SetPercentage
//-------------------------------------------------------------------
void GuiComponent_Slider::SetPercentage(float percentage)
{
	m_percentage = math::clamp(percentage, 0.0f, 1.0f);

	if ( m_config.pSliderFunction )
		m_config.pSliderFunction( this, GetValue() );
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
float GuiComponent_Slider::GetValue()const
{
	return math::lerp( m_config.minValue, m_config.maxValue, m_percentage );
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_Slider::SetConfig(const GuiConfig_Slider& config)
{
	m_config = config;
	SetPercentage( m_percentage );
	ValidateConfig();
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Slider::GetScale()const
{
	math::Vec2 result = m_config.dimension * 0.5f;

	return result;
}
//-------------------------------------------------------------------
//	IsPointOnTop
//-------------------------------------------------------------------
bool GuiComponent_Slider::IsPointOnTop(const math::Vec2& pointScreen)const
{
	return IsPointOnTopSlider(pointScreen) ||  IsPointOnTopCircle(pointScreen);
}
//-------------------------------------------------------------------
//	IsPointOnTopSlider
//-------------------------------------------------------------------
bool GuiComponent_Slider::IsPointOnTopSlider(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();
	math::Vec2 point = pointScreen;

	return math::pointOnQuad( point, pos - scale, pos + scale );
}
//-------------------------------------------------------------------
//	IsPointOnTopCircle
//-------------------------------------------------------------------
bool GuiComponent_Slider::IsPointOnTopCircle(const math::Vec2& pointScreen)const
{
	math::Vec2 circlePos = GetCirclePos();
	math::Vec2 circleScale = GetCircleScale();
	math::Vec2 pointNdc = pointScreen;

	return math::pointOnQuad( pointNdc, circlePos - circleScale, circlePos + circleScale );
}
//-------------------------------------------------------------------
//	GetCirclePos
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Slider::GetCirclePos()const
{
	math::Vec2 result = GetPos();
	math::Vec2 sliderScale = GetScale();

	result.x += math::lerp( -sliderScale.x, sliderScale.x, m_percentage );

	return result;	
}
//-------------------------------------------------------------------
//	GetCircleScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Slider::GetCircleScale()const
{
	math::Vec2 sliderScale = GetScale();

	return math::Vec2( m_config.circleScale ) + math::Vec2( sliderScale.y );
}

/****************************************************************************************
	GuiComponent_Textbox
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Textbox::GuiComponent_Textbox() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
	m_inUse = false;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Textbox::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Textbox::OnInitialize()
{
	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );
	CHECK( m_hFont );

	m_hModelTextbox = primitive::geometry::createSquare();

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetCharSize( 12 );
	m_pFontEffect->SetCharSpaceProportion( 0.05f );
	m_pFontEffect->SetColor( math::normalizeRgb(30) );

	KeyMapContext* pContext = g_pKeyMap->GetContext( editorGlobal::action::context::GUI_COMPONENT_TEXTBOX );
	if ( !pContext )
	{
		pContext = g_pKeyMap->CreateContext( editorGlobal::action::context::GUI_COMPONENT_TEXTBOX );
		
		pContext->AddAction( editorGlobal::action::LEFT_CLICK, KEY_MOUSE_LEFT, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::CONFIRM, KEY_ESCAPE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::CONFIRM, KEY_RETURN, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_BACKSPACE, KEY_BACKSPACE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_DELETE, KEY_DELETE, (uint)eKeyState::Press );
	}
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_Textbox::OnUpdate(IGuiPageBridge* pPageBridge)
{
	if ( g_pKeyMap->CanPerform(editorGlobal::action::LEFT_CLICK) )
	{
		if ( IsPointOnTop(g_pInput->GetMousePos()) )
			StartUse();
		else
			EndUse();
	}

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CONFIRM) )
		EndUse();

	if ( m_inUse )
	{
		if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_BACKSPACE) )
		{
			if ( m_value.Length() > 1 )
				m_value = string::copy( m_value, 0, m_value.Length() - 1 );
			else if ( m_value.Length() == 1 )
				m_value = String();
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_DELETE) )
		{
			SetValue( "" );
		}
		else
		{
			List<eKey> keysPressed;
			g_pInput->GetAllKeys(eKeyState::Press, keysPressed);

			for ( auto it = keysPressed.GetIterator(); it; it++ )
			{
				char cMapped = g_pInput->MapToChar( *it );
				AddChar( cMapped );
			}
		}
	}
}
//-------------------------------------------------------------------
//	StartUse
//-------------------------------------------------------------------
void GuiComponent_Textbox::StartUse()
{
	if ( !m_inUse )
	{
		g_pKeyMap->QueueActiveContexts();
		g_pKeyMap->DeactivateAllContexts();
		g_pKeyMap->UseContext( editorGlobal::action::context::GUI_COMPONENT_TEXTBOX );
		m_inUse = true;
	}
}
//-------------------------------------------------------------------
//	EndUse
//-------------------------------------------------------------------
void GuiComponent_Textbox::EndUse()
{
	if ( m_inUse )
	{
		m_inUse = false;
		SetValue( m_value );

		List<const KeyMapContext*> contexts;
		g_pKeyMap->GetQueuedContexts( contexts );

		for ( auto it = contexts.GetIterator(); it; it++ )
		{
			if ( (*it)->GetName() != editorGlobal::action::context::GUI_COMPONENT_TEXTBOX )
			{
				SmartPtr<Process_ChangeKeyMapContext> pProcess = new Process_ChangeKeyMapContext;
				pProcess->SetContext( (*it)->GetName(), true );

				g_pProcess->Add( pProcess.As<Process>() );
			}
		}

		g_pKeyMap->EndUseContext( editorGlobal::action::context::GUI_COMPONENT_TEXTBOX );

		if ( m_config.pTextboxFunction )
			m_config.pTextboxFunction( this, m_value );
	}
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Textbox::OnValidateConfig()const
{
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Textbox::Render()
{
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 boxPos = GetPos();
	math::Vec3 boxPosTransformed = math::screenCoordToCenter( boxPos, viewportDimension ).Xyz( GetZ() );
	math::Vec3 boxScale = GetScale().Xyz(1.0f);
	math::Mtx44 proj = math::matrixOrtho( viewportDimension );
	math::Mtx44 view;

	// The box part of Textbox
	{
		math::Mtx44 world = math::matrixScale( boxScale ) * math::matrixTranslate( boxPosTransformed );

		m_pEffect->GetConfig().color4 = math::Vec3(0.75f * m_inUse ? 1.0f : 0.85f).Xyzw( getAlphaForState( GetPageInfo() ) );
		m_pEffect->Render( m_hModelTextbox, world, view, proj );
	}

	// The text part of Textbox
	{
		math::Vec2 textPos = boxPos - math::scale(boxScale.Xy(), math::Vec2(0.95f, 0.7f));
		textPos.y -= 3.0f;

		m_pFontEffect->SetColor( math::Vec3(0.1f).Xyzw(getAlphaForState( GetPageInfo()) ) );
		m_pFontEffect->SetScreenPos( textPos );
		m_pFontEffect->SetZ( GetZ() - 0.01f );
		m_pFontEffect->Render( m_value, m_hFont.GetPtr() );
	}
}
//-------------------------------------------------------------------
//	AddChar
//-------------------------------------------------------------------
void GuiComponent_Textbox::AddChar(char c)
{
	if ( IsCharValid(c) && m_value.Length() < m_config.maxChars )
		m_value += c;
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
void GuiComponent_Textbox::SetValue(const String& value)
{
	String newValue;

	for ( uint i = 0; i < value.Length(); i++ )
	{
		if ( IsCharValid(value[i]) )
			newValue += value[i];
	}

	// By reference makes passing m_value to SetValue something that can go very wrong...
	m_value = newValue;
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
const String& GuiComponent_Textbox::GetValue()const
{
	return m_value;
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_Textbox::SetConfig(const GuiConfig_Textbox& config)
{
	m_config = config;
	
	ValidateConfig();
	
	m_pFontEffect->SetCharSize( m_config.charSize );
	m_pFontEffect->SetCharSpaceProportion( m_config.charSpaceProportion );
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Textbox::GetScale()const
{
	math::Vec2 result = m_config.dimension * 0.5f;

	return result;
}
//-------------------------------------------------------------------
//	IsPointOnTop
//-------------------------------------------------------------------
bool GuiComponent_Textbox::IsPointOnTop(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();
	return math::pointOnQuad( pointScreen, math::Aabb2( pos - scale, pos + scale ) );
}
//-------------------------------------------------------------------
//	IsCharValid
//-------------------------------------------------------------------
bool GuiComponent_Textbox::IsCharValid(char c)const
{
	return isCharValid(c, m_config.charsAllowed );
}

/****************************************************************************************
	GuiComponent_Numberbox
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Numberbox::GuiComponent_Numberbox() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
	m_inUse = false;
	m_valueStart = 0.0f;
	m_contextCounter = 0;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Numberbox::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Numberbox::OnInitialize()
{
	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );
	CHECK( m_hFont );

	m_hModelTextbox = primitive::geometry::createSquare();

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetCharSize( 12 );
	m_pFontEffect->SetCharSpaceProportion( 0.05f );
	m_pFontEffect->SetColor( math::normalizeRgb(30) );

	KeyMapContext* pContext = g_pKeyMap->GetContext( editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX );
	if ( !pContext )
	{
		pContext = g_pKeyMap->CreateContext( editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX );

		pContext->AddAction( editorGlobal::action::LEFT_CLICK, KEY_MOUSE_LEFT, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::CONFIRM, KEY_ESCAPE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::CONFIRM, KEY_RETURN, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_BACKSPACE, KEY_BACKSPACE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_DELETE, KEY_DELETE, (uint)eKeyState::Press );
	}
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_Numberbox::OnUpdate(IGuiPageBridge* pPageBridge)
{
	if ( g_pKeyMap->CanPerform(editorGlobal::action::LEFT_CLICK) )
	{
		if ( IsPointOnTop(g_pInput->GetMousePos()) )
			StartUse();
		else
			EndUse();
	}

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CONFIRM) )
		EndUse();

	if ( m_inUse )
	{
		if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_BACKSPACE) )
		{
			if ( m_value.Length() > 1 )
				m_value = string::copy( m_value, 0, m_value.Length() - 1 );
			else if ( m_value.Length() == 1 )
				m_value = String();
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_DELETE) )
		{
			SetValue( 0.0f );
		}
		else
		{
			List<eKey> keysPressed;
			g_pInput->GetAllKeys(eKeyState::Press, keysPressed);

			for ( auto it = keysPressed.GetIterator(); it; it++ )
			{
				char cMapped = g_pInput->MapToChar( *it );
				AddChar( cMapped );
			}
		}
	}
}
//-------------------------------------------------------------------
//	StartUse
//-------------------------------------------------------------------
void GuiComponent_Numberbox::StartUse()
{
	if ( !m_inUse ) 
	{
		g_pKeyMap->QueueActiveContexts();
		g_pKeyMap->DeactivateAllContexts();
		auto pContext = g_pKeyMap->UseContext( editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX );

		if ( pContext )
			m_contextCounter = pContext->GetUseCounter();

		m_inUse = true;
		m_valueStart = GetValidValueFloat( m_value, 0.0f );
	}
}
//-------------------------------------------------------------------
//	EndUse
//-------------------------------------------------------------------
void GuiComponent_Numberbox::EndUse()
{
	if ( m_inUse )
	{
		m_inUse = false;
		SetValue( GetValidValueFloat(m_value, m_valueStart) );

		auto pContext = g_pKeyMap->GetContext(editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX);
		if ( !pContext || m_contextCounter == pContext->GetUseCounter() )
		{
			List<const KeyMapContext*> contexts;
			g_pKeyMap->GetQueuedContexts( contexts );

			for ( auto it = contexts.GetIterator(); it; it++ )
			{
				if ( (*it)->GetName() != editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX )
				{
					SmartPtr<Process_ChangeKeyMapContext> pProcess = new Process_ChangeKeyMapContext;
					pProcess->SetContext( (*it)->GetName(), true );

					g_pProcess->Add( pProcess.As<Process>() );
				}
			}

			g_pKeyMap->EndUseContext( editorGlobal::action::context::GUI_COMPONENT_NUMBERBOX );
		}

		if ( m_config.pNumberboxFunction )
			m_config.pNumberboxFunction( this, GetValue() );
	}
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Numberbox::OnValidateConfig()const
{
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Numberbox::Render()
{
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 boxPos = GetPos();
	math::Vec3 boxPosTransformed = math::screenCoordToCenter( boxPos, viewportDimension ).Xyz( GetZ() );
	math::Vec3 boxScale = GetScale().Xyz(1.0f);
	math::Mtx44 proj = math::matrixOrtho( viewportDimension );
	math::Mtx44 view;

	// The box part of Numberbox
	{
		math::Mtx44 world = math::matrixScale( boxScale ) * math::matrixTranslate( boxPosTransformed );

		m_pEffect->GetConfig().color4 = math::Vec3(0.75f * m_inUse ? 1.0f : 0.85f).Xyzw( getAlphaForState( GetPageInfo() ) );
		m_pEffect->Render( m_hModelTextbox, world, view, proj );
	}

	// The text part of Numberbox
	{
		math::Vec2 textPos = boxPos - math::scale(boxScale.Xy(), math::Vec2(0.95f, 0.7f));
		textPos.y -= 3.0f;

		m_pFontEffect->SetColor( math::Vec3(0.1f).Xyzw(getAlphaForState( GetPageInfo()) ) );
		m_pFontEffect->SetScreenPos( textPos );
		m_pFontEffect->SetZ( GetZ() - 0.01f );
		m_pFontEffect->Render( m_value, m_hFont.GetPtr() );
	}
}
//-------------------------------------------------------------------
//	AddChar
//-------------------------------------------------------------------
void GuiComponent_Numberbox::AddChar(char c)
{
	if ( c == ',' )
		c = '.';
	if ( string::contains("-.0123456789", c) )
		m_value += c;
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
void GuiComponent_Numberbox::SetValue(float f)
{
	f = math::clamp( f, m_config.valueMin, m_config.valueMax );
	m_value = string::floatToStr( f );
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
float GuiComponent_Numberbox::GetValue()const
{
	return GetValidValueFloat( m_value, 0.0f );
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_Numberbox::SetConfig(const GuiConfig_Numberbox& config)
{
	m_config = config;
	m_config.valueMin = math::min( config.valueMin, config.valueMax );
	m_config.valueMax = math::max( config.valueMin, config.valueMax );
	
	ValidateConfig();

	m_pFontEffect->SetCharSize( m_config.charSize );
	m_pFontEffect->SetCharSpaceProportion( m_config.charSpaceProportion );
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Numberbox::GetScale()const
{
	math::Vec2 result = m_config.dimension * 0.5f;

	return result;
}
//-------------------------------------------------------------------
//	IsPointOnTop
//-------------------------------------------------------------------
bool GuiComponent_Numberbox::IsPointOnTop(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();
	return math::pointOnQuad( pointScreen, math::Aabb2( pos - scale, pos + scale ) );
}
//-------------------------------------------------------------------
//	GetValidValueFloat
//-------------------------------------------------------------------
float GuiComponent_Numberbox::GetValidValueFloat(const String& s, float defaultValue)const
{
	if ( !string::isNumber(s, 6, 6) )
		return math::clamp( defaultValue, m_config.valueMin, m_config.valueMax );

	return string::strToFloat( s );
}

/****************************************************************************************
	GuiComponent_TextArea
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_TextArea::GuiComponent_TextArea() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
	m_inUse = false;
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_TextArea::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_TextArea::OnInitialize()
{
	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );
	CHECK( m_hFont );

	m_hModelTextbox = primitive::geometry::createSquare();

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetCharSize( 14 );
	m_pFontEffect->SetCharSpaceProportion( 0.05f );
	m_pFontEffect->SetColor( math::normalizeRgb(30) );

	KeyMapContext* pContext = g_pKeyMap->GetContext( editorGlobal::action::context::GUI_COMPONENT_TEXTAREA );
	if ( !pContext )
	{
		pContext = g_pKeyMap->CreateContext( editorGlobal::action::context::GUI_COMPONENT_TEXTAREA );

		pContext->AddAction( editorGlobal::action::LEFT_CLICK, KEY_MOUSE_LEFT, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::ESCAPE, KEY_ESCAPE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_BACKSPACE, KEY_BACKSPACE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_DELETE, KEY_DELETE, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_HOME, KEY_HOME, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_END, KEY_END, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_UP, KEY_UP, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_DOWN, KEY_DOWN, (uint)eKeyState::Press );
		pContext->AddAction( editorGlobal::action::TEXT_DOWN, KEY_RETURN, (uint)eKeyState::Press );
	}
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_TextArea::OnUpdate(IGuiPageBridge* pPageBridge)
{
	if ( g_pKeyMap->CanPerform(editorGlobal::action::LEFT_CLICK) )
	{
		if ( !IsPointOnTop(g_pInput->GetMousePos()) )
			EndUse();
	}

	if ( g_pKeyMap->CanPerform(editorGlobal::action::ESCAPE) )
		EndUse();

	if ( m_inUse )
	{
		if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_BACKSPACE) )
		{
			String newStr = m_pCurrent->GetValue();
			if ( newStr.Length() > 1 )
				newStr = string::copy( newStr, 0, newStr.Length() - 1 );
			else if ( newStr.Length() == 1 )
				newStr = String();

			m_pCurrent->GetValue() = newStr;
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_DELETE) )
		{
			if ( m_pCurrent )
				m_pCurrent->GetValue() = "";

			for ( auto pNode = m_values.GetFirst(); pNode; )
			{
				auto pRemove = pNode;
				pNode = pNode->GetNext();

				if ( m_pCurrent->GetId() == pRemove->GetId() )
					continue;

				if ( pRemove->GetValue().Length() == 0 )
					pRemove->Remove();
			}
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_HOME) )
		{
			m_pCurrent = m_values.GetFirst();
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_END) )
		{
			m_pCurrent = m_values.GetLast();
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_UP) )
		{
			m_pCurrent = m_pCurrent->GetPrev();

			if ( !m_pCurrent )
				m_pCurrent = m_values.GetFirst();
		}
		else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEXT_DOWN) )
		{
			m_pCurrent = m_pCurrent->GetNext();
			
			if ( !m_pCurrent )
			{
				if ( m_values.GetLast()->GetValue().Length() > 0 )
					m_values.AddLast( "" );
				m_pCurrent = m_values.GetLast();
			}
		}
		else
		{
			List<eKey> keysPressed;
			g_pInput->GetAllKeys(eKeyState::Press, keysPressed);

			for ( auto it = keysPressed.GetIterator(); it; it++ )
			{
				char cMapped = g_pInput->MapToChar( *it );
				AddChar( cMapped );
			}
		}
	}
}
//-------------------------------------------------------------------
//	StartUse
//-------------------------------------------------------------------
void GuiComponent_TextArea::StartUse()
{
	g_pKeyMap->QueueActiveContexts();
	g_pKeyMap->DeactivateAllContexts();
	g_pKeyMap->UseContext( editorGlobal::action::context::GUI_COMPONENT_TEXTAREA );
	m_inUse = true;

	if ( m_values.Count() == 0 )
	{
		m_values.AddLast("");
		m_pCurrent = m_values.GetLast();
	}
}
//-------------------------------------------------------------------
//	EndUse
//-------------------------------------------------------------------
void GuiComponent_TextArea::EndUse()
{
	if ( m_inUse )
	{
		m_inUse = false;
		SetValue( m_values );

		List<const KeyMapContext*> contexts;
		g_pKeyMap->GetQueuedContexts( contexts );

		for ( auto it = contexts.GetIterator(); it; it++ )
		{
			if ( (*it)->GetName() != editorGlobal::action::context::GUI_COMPONENT_TEXTAREA )
			{
				SmartPtr<Process_ChangeKeyMapContext> pProcess = new Process_ChangeKeyMapContext;
				pProcess->SetContext( (*it)->GetName(), true );

				g_pProcess->Add( pProcess.As<Process>() );
			}
		}

		g_pKeyMap->EndUseContext( editorGlobal::action::context::GUI_COMPONENT_TEXTAREA );

		if ( m_config.pTextAreaFunction )
			m_config.pTextAreaFunction( this, m_values, m_config.flag );
	}
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_TextArea::OnValidateConfig()const
{
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_TextArea::Render()
{
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 boxPos = GetPos();
	math::Vec3 boxPosTransformed = math::screenCoordToCenter( boxPos, viewportDimension ).Xyz( GetZ() );
	math::Vec3 boxScale = GetScale().Xyz(1.0f);
	math::Mtx44 proj = math::matrixOrtho( viewportDimension );
	math::Mtx44 view;
	uint line = GetCurrentIndex();
	uint lineOnArea = line % m_config.linesPerArea;
	float alpha = getAlphaForState( GetPageInfo() );
	math::Vec2 boxTopLeft = boxPos - boxScale.Xy();
	float charSize = GetAdjustedCharSize();

	// The area part of TextArea
	{
		math::Mtx44 world = math::matrixScale( boxScale ) * math::matrixTranslate( boxPosTransformed );

		m_pEffect->GetConfig().color4 = math::Vec3(0.75f).Xyzw( alpha );
		m_pEffect->Render( m_hModelTextbox, world, view, proj );

		// Active line
		math::Vec3 boxActiveScale = boxScale;
		boxActiveScale.y = charSize * 0.5f;
		math::Vec2 boxActivePosScreen = boxTopLeft;
		boxActivePosScreen.x += boxScale.x;
		boxActivePosScreen.y += boxActiveScale.y + (float)lineOnArea * charSize;
		math::Vec3 boxActivePos = math::screenCoordToCenter( boxActivePosScreen, viewportDimension ).Xyz( GetZ() * 0.95f );

		world = math::matrixScale( boxActiveScale ) * math::matrixTranslate( boxActivePos );

		m_pEffect->GetConfig().color4.x *= 1.2f;
		m_pEffect->GetConfig().color4.y *= 1.2f;
		m_pEffect->GetConfig().color4.z *= 1.2f;
		m_pEffect->Render( m_hModelTextbox, world, view, proj );
	}

	// The text part of TextArea
	{
		NodeList<String>* pNode = GetLineAt( line - lineOnArea );
		float leftPadding = 10.0f;
		float bottomPadding = 3.0f;

		m_pFontEffect->SetZ( GetZ() * 0.9f );

		for ( uint i = 0; i < m_config.linesPerArea && pNode; i++, pNode = pNode->GetNext() )
		{
			math::Vec2 pos = boxTopLeft;
			pos.x += leftPadding;
			pos.y += i * charSize - bottomPadding;

			math::Vec4 color = m_pFontEffect->GetColor();
			m_pFontEffect->SetColor( color.x, color.y, color.z, alpha );
			m_pFontEffect->SetScreenPos( pos );
			m_pFontEffect->Render( pNode->GetValue(), m_hFont.GetPtr() );
		}
	}
}
//-------------------------------------------------------------------
//	AddChar
//-------------------------------------------------------------------
void GuiComponent_TextArea::AddChar(char c)
{
	CHECK( m_pCurrent );
	if ( IsCharValid(c) )
		m_pCurrent->GetValue() += c;
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
void GuiComponent_TextArea::SetValue(const List<String>& values)
{
	// This has to be one of the worst algorithms in this code base...
	List<String> newValueList;

	for ( auto it = values.GetIterator(); it; it++ )
	{
		if ( (*it).Length() == 0 )
			continue;

		String newValue = "";
		for ( uint i = 0; i < (*it).Length(); i++ )
		{
			if ( IsCharValid((*it)[i]) )
				newValue += (*it)[i];
		}

		if ( newValue.Length() > 0 )
			newValueList.AddLast( newValue );
	}

	// ...It gets even worse
	m_values.Clear();

	for ( auto it = newValueList.GetIterator(); it; it++ )
		m_values.AddLast( *it );

	if ( m_values.Count() == 0 )
		m_values.AddLast("");

	m_pCurrent = m_values.GetFirst();
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
const List<String>& GuiComponent_TextArea::GetValue()const
{
	return m_values;
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_TextArea::SetConfig(const GuiConfig_TextArea& config)
{
	m_config = config;

	ValidateConfig();

	m_pFontEffect->SetCharSize( m_config.charSize );
	m_pFontEffect->SetCharSpaceProportion( m_config.charSpaceProportion );
}
//-------------------------------------------------------------------
//	GetAdjustedCharSize
//-------------------------------------------------------------------
float GuiComponent_TextArea::GetAdjustedCharSize()const
{
	return (float)m_pFontEffect->GetCharSize() * 1.1f;
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_TextArea::GetScale()const
{
	math::Vec2 dimension(g_pRenderer->GetWindowViewport().width * 0.7f, GetAdjustedCharSize() * (float)m_config.linesPerArea);

	return dimension * 0.5f;
}
//-------------------------------------------------------------------
//	IsPointOnTop
//-------------------------------------------------------------------
bool GuiComponent_TextArea::IsPointOnTop(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();
	return math::pointOnQuad( pointScreen, math::Aabb2( pos - scale, pos + scale ) );
}
//-------------------------------------------------------------------
//	IsCharValid
//-------------------------------------------------------------------
bool GuiComponent_TextArea::IsCharValid(char c)const
{
	return isCharValid(c, m_config.charsAllowed );
}
//-------------------------------------------------------------------
//	GetCurrentIndex
//-------------------------------------------------------------------
uint GuiComponent_TextArea::GetCurrentIndex()const
{
	uint result = 0;

	for ( auto pNode = m_values.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( m_pCurrent->GetId() == pNode->GetId() )
			break;

		result++;
	}

	return result;
}
//-------------------------------------------------------------------
//	GetLineAt
//-------------------------------------------------------------------
NodeList<String>* GuiComponent_TextArea::GetLineAt(uint index)
{
	NodeList<String>* pNode = m_values.GetFirst();

	for ( uint i = 0; i < index; i++ )
		pNode = pNode->GetNext();

	return pNode;
}
//-------------------------------------------------------------------
//	OnShow
//-------------------------------------------------------------------
void GuiComponent_TextArea::OnShow()
{
	StartUse();
}

/****************************************************************************************
	GuiComponent_Checkbox
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GuiComponent_Checkbox::GuiComponent_Checkbox() : GuiComponent_Base(&m_config, 1.0f, 0.0f, -1.0f)
{
	m_value = false;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GuiComponent_Checkbox::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GuiComponent_Checkbox::OnInitialize()
{
	m_hModel = primitive::geometry::createSquare();

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;
}
//-------------------------------------------------------------------
//	OnUpdate
//-------------------------------------------------------------------
void GuiComponent_Checkbox::OnUpdate(IGuiPageBridge* pPageBridge)
{
	math::Vec2 mousePos = g_pInput->GetMousePos();
	bool canUse = CanUse(pPageBridge);
	bool isPointOnTop = IsPointOnTop(mousePos);
	bool isKeyPress = g_pInput->IsKeyPress(KEY_MOUSE_LEFT);

	if ( canUse && isPointOnTop && isKeyPress )
	{
		SetValue( !m_value );
	}
}
//-------------------------------------------------------------------
//	OnValidateConfig
//-------------------------------------------------------------------
void GuiComponent_Checkbox::OnValidateConfig()const
{
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GuiComponent_Checkbox::Render()
{
	float z = GetZ();
	math::Vec2 screenPos = GetPos();
	math::Vec3 pos = math::screenCoordToCenter( screenPos, g_pRenderer->GetWindowViewport().GetDimension() ).Xyz( z );
	math::Vec3 scale = GetScale().Xyz(1.0f);
	float alpha = getAlphaForState(GetPageInfo());
	math::Mtx44 proj = math::matrixOrtho( g_pRenderer->GetWindowViewport().GetDimension() );
	math::Mtx44 view;

	{
		math::Mtx44 mtxTranslate = math::matrixTranslate( pos );
		math::Mtx44 world = math::matrixScale( scale ) * mtxTranslate;

		m_pEffect->GetConfig().color4 = m_config.backgroundColor3.Xyzw( alpha );
		m_pEffect->Render( m_hModel, world, view, proj );
	}

	if ( m_value )
	{
		math::Mtx44 world = math::matrixScale( scale * 0.75f ) * math::matrixTranslate( pos.x, pos.y, pos.z * 0.99f );

		m_pEffect->GetConfig().color4 = m_config.selectedColor3.Xyzw( alpha );
		m_pEffect->Render( m_hModel, world, view, proj );
	}
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
void GuiComponent_Checkbox::SetValue(bool value)
{
	m_value = value;

	if ( m_config.pCheckboxFunction )
		m_config.pCheckboxFunction( this, m_value );
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
bool GuiComponent_Checkbox::GetValue()const
{
	return m_value;
}
//-------------------------------------------------------------------
//	SetConfig
//-------------------------------------------------------------------
void GuiComponent_Checkbox::SetConfig(const GuiConfig_Checkbox& config)
{
	m_config = config;
	ValidateConfig();
}
//-------------------------------------------------------------------
//	GetScale
//-------------------------------------------------------------------
math::Vec2 GuiComponent_Checkbox::GetScale()const
{
	math::Vec2 result = math::Vec2( m_config.dimension * 0.5f );

	return result;
}
//-------------------------------------------------------------------
//	IsPointOnTop
//-------------------------------------------------------------------
bool GuiComponent_Checkbox::IsPointOnTop(const math::Vec2& pointScreen)const
{
	math::Vec2 pos = GetPos();
	math::Vec2 scale = GetScale();

	return math::pointOnQuad( pointScreen, math::Aabb2(pos - scale, pos + scale) );
}
