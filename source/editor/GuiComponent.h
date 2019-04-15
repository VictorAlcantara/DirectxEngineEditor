#pragma once

#include "../engine/Pointer.h"
#include "../engine/List.h"
#include "../engine/Handle.h"
#include "../engine/Gradient.h"
#include "../engine/Handle.h"
#include "../engine/String.h"
#include "GuiUtil.h"
#include "GuiConfig.h"

class Effect_Blur;
class Effect_Texture;
class Effect_Color;
class Effect_LightDir4;
class Model;
class RenderTarget;
class ITexture;
class Texture2d;
class IGuiAnimation;
class GuiPage;
class GuiAnimation;
class Font;
class FontEffect_Color;
class IGuiPageBridge;

#define GUICOMPONENT_TYPE(type) public: \
	eGuiComponent GetType()const override final { return type; } \
	static eGuiComponent GetTypeStatic() { return type; } \
	friend class GuiPage;

/****************************************************************************************
	IGuiComponent
*****************************************************************************************/

class IGuiComponent
{
	friend class GuiPage;

	public:
		virtual ~IGuiComponent() = default;

		virtual void Initialize(const HashString& name) = 0;
		virtual void Shutdown() = 0;

		
		virtual uint GetId()const = 0;
		virtual const HashString& GetName()const = 0;

		virtual void Update(IGuiPageBridge*) = 0;
		virtual void Render() = 0;

		virtual eGuiComponent GetType()const = 0;

		virtual const List<IGuiComponent*>& GetAllChildren()const = 0;
		virtual void AddChild(IGuiComponent*) = 0;
		virtual void RemoveChild(const HashString& componentName) = 0;
		virtual void RemoveAllChildren() = 0;

		virtual void SetParent(IGuiComponent*) = 0;

		virtual bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const = 0;

		virtual math::Vec2 GetPos()const = 0;
		virtual math::Vec2 GetScale()const = 0;

		virtual const String& GetCommand()const = 0;

	protected:
		virtual void OnShow() = 0;
		virtual void OnHide() = 0;
};

/****************************************************************************************
	GuiComponent_Base
*****************************************************************************************/

class GuiComponent_Base : public IGuiComponent
{
	public:
		virtual ~GuiComponent_Base();

		uint GetId()const { return m_id; }
		const HashString& GetName()const override final { return m_name; }

		void Initialize(const HashString&)override final;
		void Shutdown()override final;

		void Update(IGuiPageBridge*)override final;

		const List<IGuiComponent*>& GetAllChildren()const override final;
		void AddChild(IGuiComponent*)override final;
		void RemoveChild(const HashString& componentName)override final;
		void RemoveAllChildren()override final;

		void SetParent(IGuiComponent*)override final;

		bool IsPointOnTop(const math::Vec2& pointScreen)const override;

		math::Vec2 GetPos()const override final;

		void SetInUse(IGuiPageBridge*, bool);
		bool CanUse(IGuiPageBridge*)const;

		const String& GetCommand()const override final { return GetConfigBase().command; }

	protected:
		GuiComponent_Base(GuiConfig_Base*, float posScaleLeftTop, float posScaleCenter, float posScaleRightBottom);
		GuiComponent_Base(GuiConfig_Base*, float posScaleLeft, float posScaleTop, float posScaleCenterHorizontal, float posScaleCenterVertical, float posScaleRight, float posScaleBottom);
		float GetZ()const;

		virtual void OnInitialize() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate(IGuiPageBridge*) {}

		void OnShow()override {}
		void OnHide()override {}

		const GuiPageInfo& GetPageInfo()const { return m_pageInfo; }

		void ValidateConfig()const;
		virtual void OnValidateConfig()const = 0;

		math::Vec4 GetAnimationColor(const GuiAnimation*, const math::Vec4& color)const;
		math::Mtx44 GetAnimationTransform(const GuiAnimation*)const;

	private:
		GuiConfig_Base& GetConfigBase() { return *m_pConfigBase; }
		const GuiConfig_Base& GetConfigBase()const { return *m_pConfigBase; }

		bool m_initialized;
		uint m_id;
		HashString m_name;
		IGuiComponent* m_pParent;
		List<IGuiComponent*> m_children;
		GuiPageInfo m_pageInfo;

		GuiConfig_Base* m_pConfigBase;

		float m_posScaleLeft;
		float m_posScaleTop;
		float m_posScaleCenterHorizontal;
		float m_posScaleCenterVertical;
		float m_posScaleRight;
		float m_posScaleBottom;

		static uint s_idControl;
};

/****************************************************************************************
	GuiComponent_Panel
*****************************************************************************************/

class GuiComponent_Panel : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Panel );

	public:
		GuiComponent_Panel();
		virtual ~GuiComponent_Panel() = default;

		void Render()override final;

		void SetConfig(const GuiConfig_Panel& config) { m_config = config; ValidateConfig(); }
		const GuiConfig_Panel& GetConfig()const { return m_config; }

		math::Vec2 GetScale()const override final;

		math::Mtx33 GetTextureTransform()const;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;
		void OnValidateConfig()const override final;

	private:
		GuiConfig_Panel m_config;

		Handle<Model> m_hModel;
		SimplePtr<RenderTarget> m_pRenderTarget;
		SimplePtr<Effect_Blur> m_pEffectBlur;
		SimplePtr<Effect_Texture> m_pEffectTexture;
};

/****************************************************************************************
	GuiComponent_Button
*****************************************************************************************/

class GuiComponent_Button : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Button );

	public:
		GuiComponent_Button();
		virtual ~GuiComponent_Button() = default;

		void Render()override final;

		void SetConfig(const GuiConfig_Button& config) { m_config = config; ValidateConfig(); }
		const GuiConfig_Button& GetConfig()const { return m_config; }

		math::Vec2 GetScale()const override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

	private:
		GuiConfig_Button m_config;
		
		Handle<Model> m_hButtonModel;
		math::Vec4 m_color;
		Handle<ITexture> m_hTextureDefault;
		SimplePtr<Effect_Texture> m_pEffectTexture;
		math::Euler m_modelRotation;
		SimplePtr<Effect_LightDir4> m_pEffectModel;
};

/****************************************************************************************
	GuiComponent_Text
*****************************************************************************************/

class GuiComponent_Text : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Text );

	public:
		GuiComponent_Text();
		virtual ~GuiComponent_Text() = default;

		void Render()override final;

		void SetConfig(const GuiConfig_Text& config);
		const GuiConfig_Text& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final { return false; }

		math::Vec2 GetScale()const override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnValidateConfig()const override final;

	private:
		GuiConfig_Text m_config;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;
};

/****************************************************************************************
	GuiComponent_Slider
*****************************************************************************************/

class GuiComponent_Slider : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Slider );

	public:
		GuiComponent_Slider();
		virtual ~GuiComponent_Slider() = default;

		void Render()override final;
		
		void SetValue(float value);
		void SetPercentage(float);
		float GetValue()const;
		float GetPercentage()const { return m_percentage; }

		void SetConfig(const GuiConfig_Slider&);
		const GuiConfig_Slider& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final;
		bool IsPointOnTopSlider(const math::Vec2& pointScreenNormalized)const;
		bool IsPointOnTopCircle(const math::Vec2& pointScreenNormalized)const;

		math::Vec2 GetScale()const override final;
		math::Vec2 GetCirclePos()const;
		math::Vec2 GetCircleScale()const;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

	private:
		void UpdateSaturation(IGuiPageBridge*);

		GuiConfig_Slider m_config;

		Handle<Model> m_hModelSlider;
		Handle<Model> m_hModelCircle;
		SimplePtr<Effect_Color> m_pEffect;
		
		float m_saturate;
		bool m_inUse;

		float m_percentage;
};

/****************************************************************************************
	GuiComponent_Textbox
*****************************************************************************************/

class GuiComponent_Textbox : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Textbox );

	public:
		GuiComponent_Textbox();
		virtual ~GuiComponent_Textbox() = default;

		void Render()override final;

		void AddChar(char);
		void SetValue(const String&);
		const String& GetValue()const;

		void SetConfig(const GuiConfig_Textbox&);
		const GuiConfig_Textbox& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final;

		math::Vec2 GetScale()const override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

	private:
		bool IsCharValid(char)const;	
		void StartUse();
		void EndUse();

		GuiConfig_Textbox m_config;

		bool m_inUse;
		String m_value;

		Handle<Model> m_hModelTextbox;
		SimplePtr<Effect_Color> m_pEffect;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;
};

/****************************************************************************************
	GuiComponent_Numberbox
*****************************************************************************************/

class GuiComponent_Numberbox : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Numberbox );

	public:
		GuiComponent_Numberbox();
		virtual ~GuiComponent_Numberbox() = default;

		void Render()override final;

		void AddChar(char);
		void SetValue(float);
		float GetValue()const;

		void SetConfig(const GuiConfig_Numberbox&);
		const GuiConfig_Numberbox& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final;

		math::Vec2 GetScale()const override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

	private:
		float GetValidValueFloat(const String&, float defaultValue)const;
		void StartUse();
		void EndUse();

		GuiConfig_Numberbox m_config;

		bool m_inUse;
		String m_value;
		float m_valueStart;
		uint m_contextCounter;

		Handle<Model> m_hModelTextbox;
		SimplePtr<Effect_Color> m_pEffect;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;
};

/****************************************************************************************
	GuiComponent_TextArea
*****************************************************************************************/

class GuiComponent_TextArea : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::TextArea );

	public:
		GuiComponent_TextArea();
		virtual ~GuiComponent_TextArea() = default;

		void Render()override final;

		void AddChar(char);
		void SetValue(const List<String>&);
		const List<String>& GetValue()const;

		void SetConfig(const GuiConfig_TextArea&);
		const GuiConfig_TextArea& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final;

		math::Vec2 GetScale()const override final;

		void SetFlag(uint flag) { m_config.flag = flag; }
		uint GetFlag()const { return m_config.flag; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

		void OnShow()override final;

	private:
		void StartUse();
		void EndUse();
		bool IsCharValid(char)const;
		uint GetCurrentIndex()const;
		NodeList<String>* GetLineAt(uint index);

		float GetAdjustedCharSize()const;

		GuiConfig_TextArea m_config;

		bool m_inUse;
		List<String> m_values;
		NodeList<String>* m_pCurrent;

		Handle<Model> m_hModelTextbox;
		SimplePtr<Effect_Color> m_pEffect;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;
};

/****************************************************************************************
	GuiComponent_Checkbox
*****************************************************************************************/

class GuiComponent_Checkbox : public GuiComponent_Base
{
	GUICOMPONENT_TYPE( eGuiComponent::Checkbox );

	public:
		GuiComponent_Checkbox();
		virtual ~GuiComponent_Checkbox() = default;

		void Render()override final;

		void SetValue(bool);
		bool GetValue()const;

		void SetConfig(const GuiConfig_Checkbox&);
		const GuiConfig_Checkbox& GetConfig()const { return m_config; }

		bool IsPointOnTop(const math::Vec2& pointScreenNormalized)const override final;

		math::Vec2 GetScale()const override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void OnUpdate(IGuiPageBridge*)override final;
		void OnValidateConfig()const override final;

	private:
		GuiConfig_Checkbox m_config;

		Handle<Model> m_hModel;
		SimplePtr<Effect_Color> m_pEffect;

		bool m_value;
};
