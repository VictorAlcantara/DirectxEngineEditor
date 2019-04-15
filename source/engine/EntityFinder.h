#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"

class Entity;
class EntityManager;

class EntityFinder : public NonCopyable
{
	friend class EntityManager;

	public:
		virtual ~EntityFinder();

		virtual void Add(Handle<Entity>) = 0;
		virtual void Remove(Handle<Entity>) = 0;
		virtual void Clear() = 0;

		virtual Handle<Entity> Find(uint flag, uint key) = 0;

		bool IsInitialized()const { return m_initialized; }

	protected:
		EntityFinder();

		virtual void OnInitialize() {}
		virtual void OnShutdown() {}
		virtual void Update() {}

	private:
		void Initialize();
		void Shutdown();

		bool m_initialized;
};
