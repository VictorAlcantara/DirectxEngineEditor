#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "../engine/Pointer.h"
#include "../engine/List.h"
#include "../engine/Chronometer.h"
#include "../engine/Math.h"

class Entity;
class Font;
class FontEffect_Color;
class Model;
class Effect_Color;
class ICommand;

namespace consoleGlobal
{
	namespace action
	{
		namespace action
		{
			extern const HashString CONSOLE_CONTEXT;
		};

		extern const HashString TOGGLE;
		extern const HashString HIDE;
		extern const HashString ENTER;
		extern const HashString TEXT_DELETE;
		extern const HashString TEXT_BACKSPACE;
		extern const HashString LINE_PREV;
		extern const HashString LINE_NEXT;
	};
};

enum class eConsoleLog
{
	Success = 0,
	Warning,
	Error,

	Count
};

class Console : public NonCopyable, public PublicModule
{
	private:
		static const math::Vec4 s_logColors[(uint)eConsoleLog::Count];

		struct ConsoleLog
		{
			eConsoleLog type;
			String msg;

			ConsoleLog() : type(eConsoleLog::Error) {}
		};

	public:
		Console();
		virtual ~Console() = default;

		void Update();
		void Render();

		void Show(bool);

		void SetCommand(SmartPtr<ICommand>);
		void AddLog(const String&, eConsoleLog);
		void ExecuteCommand(const String&);

	protected:
		void OnInitialize();
		void OnShutdown();

	private:
		SmartPtr<ICommand> m_pCommand;

		bool m_inUse;
		String m_command;

		Handle<Entity> m_hEntitySelected;

		Handle<Model> m_hModel;
		SimplePtr<Effect_Color> m_pEffect;
		Handle<Font> m_hFont;
		SimplePtr<FontEffect_Color> m_pFontEffect;

		ListIterator<String> m_iterator;
		List<String> m_queue;

		ConsoleLog m_logs[10];
		uint m_logCurrent;
		float m_logTimer;
		float m_logDuration;
};
