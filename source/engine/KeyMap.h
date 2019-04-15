#pragma once

#include "CoreModule.h"
#include "../engine/Class.h"
#include "../engine/String.h"
#include "../engine/List.h"
#include "Input.h"
#include "../engine/Table.h"

class KeyMap;
class KeyMapContext;
class KeyMapGroup;

/****************************************************************************************
	class KeyMapObject
*****************************************************************************************/

class KeyMapObject
{
	friend class KeyMapContext;
	friend class KeyMapGroup;

	private:
		struct KeyMapNode
		{
			eKey key;
			uint stateKeyFlag;

			KeyMapNode() = default;
			KeyMapNode(eKey k, uint flag) : key(k), stateKeyFlag(flag) {}
		};

	public:
		KeyMapObject();
		~KeyMapObject()	= default;

		KeyMapObject* Link(eKey, uint keyStateFlag);
		bool CanPerform()const;
		bool HasKey(eKey)const;

	private:
		KeyMapObject(eKey, uint);

		SmartPtr<List<KeyMapNode>> m_nodes;
};

/****************************************************************************************
	class KeyMapGroup
*****************************************************************************************/

class KeyMapGroup : public NonCopyable
{
	friend class KeyMapContext;

	public:
		~KeyMapGroup() = default;
		
	private:
		KeyMapGroup() = default;

		KeyMapObject* AddKeyMapObject(eKey, uint keyStateFlag);
		bool CanPerform()const;

		List<KeyMapObject> m_objects;
};

/****************************************************************************************
	class KeyMapContext
*****************************************************************************************/

class KeyMapContext : public NonCopyable
{
	friend class KeyMap;

	public:
		KeyMapObject* AddAction(const HashString& action, eKey, uint keyStateFlag);
		void RemoveAction(const HashString& action);
		
		bool CanPerform(const HashString&)const;

		const HashString& GetName()const { return m_name; }

		void Enable() { m_enabled = true; }
		void Disable() { m_enabled = false; }

		uint GetUseCounter()const { return m_useCounter; }
		bool IsEnabled()const { return m_enabled; }

	private:
		KeyMapContext() = delete;
		KeyMapContext(const HashString&);

		KeyMapGroup* GetGroup(const HashString& action);
		const KeyMapGroup* GetGroup(const HashString& action)const;

		bool m_enabled;
		HashString m_name;
		uint m_useCounter;

		Table<SmartPtr<KeyMapGroup>, HashString> m_tableGroup;
};

/****************************************************************************************
	class KeyMap
*****************************************************************************************/

class KeyMap : public NonCopyable, public ProtectedModule
{
	CORE_MODULE;

	public:
		virtual ~KeyMap() = default;

		void Clear();

		bool CanPerform(const HashString&)const;

		KeyMapContext* CreateContext(const HashString&);
		void RemoveContext(const HashString&);
		KeyMapContext* UseContext(const HashString&);
		void EndUseContext(const HashString&);
		KeyMapContext* GetContext(const HashString&);

		void RemoveFromQueue(const HashString&);
		void QueueActiveContexts();
		void DeactivateAllContexts();
		void GetQueuedContexts(List<const KeyMapContext*>& outResult)const;

	protected:
		void OnInitialize()override final {}
		void OnShutdown()override final {}
	
	private:
		KeyMap();

		Table<SmartPtr<KeyMapContext>, HashString> m_tableContext;
		List<SmartPtr<KeyMapContext>> m_queue;
};
