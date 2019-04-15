#pragma once

#include "../engine/Handle.h"
#include "../engine/Entity.h"
#include "../engine/Event.h"
#include "CoreModule.h"
#include "../engine/Table.h"
#include "../engine/EntityFinder.h"

/****************************************************************************************
	Enums
*****************************************************************************************/

enum class eEventEntity
{
	None,
	Create,
	Remove,
	RemoveAll,
};

/****************************************************************************************
	Struct declaration
*****************************************************************************************/

struct Event_EntityManager : public Event
{
	EVENT_DECL(Event_EntityManager);

	eEventEntity action;
	Handle<Entity> hEntity;

	Event_EntityManager() : action(eEventEntity::None) {}
};

/****************************************************************************************
	EntityManager declaration
*****************************************************************************************/

class EntityManager : public NonCopyable
{
	CORE_MODULE;

	public:
		~EntityManager();
		
		void Update();

		template <class TYPE>
		Handle<Entity> Create();

		template <class TYPE>
		TYPE* CreateGetPtr();
		
		void Remove(uint id);
		void Remove(Handle<Entity>);
		void RemoveNextFrame(uint id);
		void RemoveNextFrame(Handle<Entity>);
		void Clear();

		TableIterator<Handle<Entity>> GetIterator();
		TableIteratorConst<Handle<Entity>> GetIterator()const;

		void SetEntityFinder(EntityFinder*);
		EntityFinder* GetEntityFinder();

		void ChangeName(Handle<Entity>, const HashString& newName);

		Handle<Entity> GetById(uint id);
		TableIterator<Handle<Entity>> GetByName(const HashString& name);
		TableIterator<Handle<Entity>> GetByClassTag(const HashString& classTag);

		uint CountName(const HashString& name)const;
		uint CountClassTag(const HashString& name)const;

	private:
		EntityManager();
		void Initialize();
		void Shutdown();

		void OnCreate(Handle<Entity>);
		void SendEvent(SmartPtr<Event_EntityManager>);
		void ClearEntityFinder();

		void AddToClassTagTable(Handle<Entity>);
		void RemoveFromClassTagTable(Handle<Entity>);

		bool m_initialized;
		HandleManager<Entity> m_handles;
		Table<Handle<Entity>, uint> m_tableEntity;
		Table<SmartPtr<Table<Handle<Entity>, uint>>, HashString> m_tableEntityName;
		Table<SmartPtr<Table<Handle<Entity>, uint>>, HashString> m_tableEntityClassTag;
		Table<Handle<Entity>, uint> m_tableUpdate;
		List<uint> m_scheduleRemove;
		SmartPtr<EntityFinder> m_pFinder;
};

//=============================================================================
//	Template method definition
//=============================================================================

//-------------------------------------------------------------------
//	Create
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<Entity> EntityManager::Create()
{
	Handle<Entity> hResult = m_handles.CreateAs<TYPE>();
	m_tableEntity.Add( hResult, hResult->GetId() );

	OnCreate( hResult );

	return hResult;
}
//-------------------------------------------------------------------
//	CreateGetPtr
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* EntityManager::CreateGetPtr()
{
	Handle<Entity> hResult = Create<TYPE>();
	return (TYPE*)hResult.GetPtr();
}
