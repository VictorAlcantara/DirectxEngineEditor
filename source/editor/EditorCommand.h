#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "../shared/Command.h"

class Entity;
class EditorWorld;

class EditorCommand : public ICommand, public NonCopyable, public ProtectedModule
{
	friend class EditorWorld;

	public:
		EditorCommand();
		virtual ~EditorCommand() = default;

		void SetWorld(EditorWorld* pWorld) { m_pWorld = pWorld; }

		bool Execute(const String&)override final;
		const String& GetLastError()const override final { return m_lastError; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		String m_lastError;
		EditorWorld* m_pWorld;
};
