#pragma once

#include "../engine/Process.h"
#include "../engine/String.h"

class Process_ChangeKeyMapContext : public Process
{
	public:
		Process_ChangeKeyMapContext();
		virtual ~Process_ChangeKeyMapContext() = default;
		void SetContext(const HashString& contextName, bool enable);

	protected:
		void Update()override final;

	private:
		HashString m_contextName;
		bool m_enable;
};

