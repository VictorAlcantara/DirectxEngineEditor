#pragma once

#include "../engine/Process.h"
#include "../engine/String.h"
#include "../engine/Handle.h"
#include "../engine/Class.h"
#include "../engine/Math.h"
#include "../engine/Chronometer.h"

class GameGui;
class Effect_Color;
class FontEffect_Color;
class Model;
class Font;
class Entity;

enum class eGuiMode
{
	None,
	Message,
	FullScreenMessage,
};

/****************************************************************************************
	IGameGuiMode
*****************************************************************************************/

class IGameGuiMode : public ProtectedModule
{
	friend class GameGui;
	
	public:
		IGameGuiMode();
		virtual ~IGameGuiMode() = default;
		virtual eGuiMode GetType()const = 0;

		virtual void Interrupt() = 0;

		uint GetId()const { return m_id; }
	protected:
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void SetOwner(GameGui* pOwner) { m_pOwner = pOwner; }

		static uint s_idControl;
		
		GameGui* m_pOwner;
		uint m_id;
};

/****************************************************************************************
	GameGuiMode_Message
*****************************************************************************************/

class GameGuiMode_Message : public IGameGuiMode
{
	friend class GameGui;

	public:
		GameGuiMode_Message();
		virtual ~GameGuiMode_Message() = default;

		void Interrupt()override final;

		void SetFlag(uint flag) { m_flag = flag; }
		void SetText(const String& text);
		void SetTextSpeed(float wordsPerSecond) { m_wps = wordsPerSecond; }

		uint GetFlag()const { return m_flag; }

		eGuiMode GetType()const override final { return eGuiMode::Message; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void Update()override final;
		void Render()override final;

	private:
		String m_text;

		Chronometer m_chronometer;
		float m_wps;
		float m_duration;

		uint m_flag;

		Handle<Model> m_hModel;
		SimplePtr<Effect_Color> m_pEffectColor;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;
};

/****************************************************************************************
	GameGuiMode_FullScreenMessage
*****************************************************************************************/

class GameGuiMode_FullScreenMessage : public IGameGuiMode
{
	friend class GameGui;

	public:
		GameGuiMode_FullScreenMessage();
		virtual ~GameGuiMode_FullScreenMessage() = default;

		void Interrupt()override final;

		void SetText(const String& text);
		void SetTextSpeed(float wordsPerSecond) { m_wps = wordsPerSecond; }

		eGuiMode GetType()const override final { return eGuiMode::FullScreenMessage; }

		void OnEnd(const String& cmd);

		void CanInterrupt(bool b) { m_canInterrupt = b; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

		void Update()override final;
		void Render()override final;

	private:
		bool m_canInterrupt;
		String m_text;

		Chronometer m_chronometer;
		float m_wps;
		float m_duration;

		Handle<Model> m_hModel;
		SimplePtr<Effect_Color> m_pEffectColor;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;

		String m_cmdEnd;
};

/****************************************************************************************
	GameGui
*****************************************************************************************/

class GameGui : public Process
{
	public:
		GameGui();
		virtual ~GameGui();

		void Render();

		IGameGuiMode* ChangeMode(eGuiMode);

		IGameGuiMode* GetMode() { return m_pMode.Get(); }
		const IGameGuiMode* GetMode()const { return m_pMode.Get(); }

	protected:
		virtual void OnEnter()override final;
		virtual void Update()override final;
		virtual void OnExit()override final;

	private:
		SimplePtr<IGameGuiMode> m_pMode;
};

/****************************************************************************************
	Process_GuiDisplayRadiusMonitor
*****************************************************************************************/

class Process_GuiDisplayRadiusMonitor : public Process
{
	public:
		Process_GuiDisplayRadiusMonitor();
		virtual ~Process_GuiDisplayRadiusMonitor() = default;

		void SetLockPos(const math::Vec3& lockPos) { m_lockPos = lockPos; }
		void SetTarget(Handle<Entity>, IGameGuiMode*);
		void SetRadius(float r) { m_radius = r; }
		void SetGui(SmartPtr<GameGui> pGui) { m_pGui = pGui; }

	protected:
		void Update()override final;

	private:
		SmartPtr<GameGui> m_pGui;
		float m_radius;
		math::Vec3 m_lockPos;
		Handle<Entity> m_hTarget;
		uint m_idGuiMode;
};
