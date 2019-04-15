#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "../shared/Command.h"

class Entity;
class GameWorld;

class GameCommand : public ICommand, public NonCopyable, public ProtectedModule
{
	friend class GameWorld;

	public:
		GameCommand();
		virtual ~GameCommand() = default;

		void SetWorld(GameWorld*);

		bool Execute(const String&)override final;
		const String& GetLastError()const override final { return m_lastError; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void ClearGetData();

		String m_lastError;
		HashString m_entitySelectedName;
		HashString m_entitySelectedTag;
		GameWorld* m_pWorld;
};

