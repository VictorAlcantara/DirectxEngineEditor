#pragma once

#include "../engine/Class.h"
#include "../engine/EventListener.h"
#include "../engine/Handle.h"
#include "../engine/Entity.h"
#include "../engine/Table.h"
#include "EditorEntityUtil.h"

class EditorEntityFinder : public NonCopyable, public EventListener
{
	public:
		EditorEntityFinder();
		~EditorEntityFinder();

		void Initialize();
		void Shutdown();
		void Clear();

		void Add(Handle<Entity>);
		void Remove(Handle<Entity>);

		void Update();

		void HandleEvent(Event*)override final;

		Handle<Entity> Find(uint flag, uint key);

	private:
		Table<Handle<Entity>, uint>* GetTable(uint flag);

		bool m_initialized;
		
		Table<Handle<Entity>, uint> m_tableCameraTrack;
		Table<Handle<Entity>, uint>* m_tables[(uint)eEditorEntityType::Count];
};
