#pragma once

#include "CoreModule.h"
#include "../engine/Class.h"
#include "../engine/Process.h"
#include "../engine/Pointer.h"
#include "../engine/Table.h"

class ProcessManager : public NonCopyable
{
	CORE_MODULE;

	public:
		~ProcessManager();

		void Initialize();
		void Shutdown();

		void Add(SmartPtr<Process>);
		void Remove(uint processId);

		void Clear();

		void Update();

	private:
		ProcessManager();

		bool m_initialized;
		Table<SmartPtr<Process>, uint> m_tableProcess;
		List<SmartPtr<Process>> m_listRemove;
};
