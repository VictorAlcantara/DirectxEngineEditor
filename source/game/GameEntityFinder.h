#pragma once

#include "../engine/EntityFinder.h"
#include "../engine/Table.h"

class GameEntityFinder : public EntityFinder
{
	public:
		GameEntityFinder();
		virtual ~GameEntityFinder();

		virtual void Add(Handle<Entity>)override final;
		virtual void Remove(Handle<Entity>)override final;
		virtual void Clear();

		virtual Handle<Entity> Find(uint flag, uint key);

	private:
		Table<Handle<Entity>, uint> m_tableCameraTrack;
};

