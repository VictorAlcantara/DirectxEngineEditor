#pragma once

#include "../engine/Class.h"
#include "Type.h"
#include"RenderInfo.h"
#include "../engine/CollisionInfo.h"
#include "../engine/Handle.h"
#include "../engine/PhysicsInfo.h"
#include "../engine/Component.h"
#include "../engine/Table.h"

class ScriptLine;

/****************************************************************************************
	Helper function
*****************************************************************************************/

math::Vec3 entityGet_pos(const Handle<Entity>&);
math::Vec3 entityGet_velocity(const Handle<Entity>&);

/****************************************************************************************
	Entity
*****************************************************************************************/

class Entity : public NonCopyable, public ClassStructure
{
	friend class EntityManager;
	friend void Component::UpdateEveryFrame(bool);

	CLASS_TAG( Entity );

	public:
		virtual ~Entity() = default;

		void SetName(const HashString&);
		const HashString& GetName()const { return m_name; }

		uint GetId()const { return m_id; }

		//RenderInfo* GetRenderInfo();
		//const RenderInfo* GetRenderInfo()const;

		//template <class TYPE>
		//TYPE* GetRenderInfoAs();

		//template <class TYPE>
		//const TYPE* GetRenderInfoAs()const;

		//void OnRender();

		virtual String GetSerialized()const;
		void ProcessCommand(const String&);
		virtual void ProcessCommand(const ScriptLine*);

		template <class TYPE>
		TYPE* Convert();
		template <class TYPE>
		const TYPE* Convert()const;

		bool MustUpdate()const { return m_mustUpdate; }

		//void UsePhysics(bool);
		//virtual void OnPhysicsUpdate(Contact*, uint contactCount) {}
		//PhysicsInfo* GetPhysicsInfo() { return &m_physicsInfo; }
		//const PhysicsInfo* GetPhysicsInfo()const { return &m_physicsInfo; }

		Handle<Entity> GetHandle();

		template <class ENTITY_TYPE>
		static ENTITY_TYPE* Convert(Handle<Entity>&);

		template <class TYPE_COMPONENT>
		Handle<Component> AddComponent();

		template <class TYPE_COMPONENT>
		HandleAs<Component, TYPE_COMPONENT> AddComponentAs();
		
		template <class TYPE_COMPONENT>
		bool RemoveComponentByType();

		bool RemoveComponent(uint id);
		bool RemoveComponent(Handle<Component>&);

		template <class COMPONENT_TYPE>
		Handle<Component> GetComponent();

		template <class COMPONENT_TYPE>
		HandleAs<Component, COMPONENT_TYPE> GetComponentAs();

		template <class TYPE_COMPONENT>
		TYPE_COMPONENT* GetComponentPtrAs();

		template <class TYPE_COMPONENT>
		const TYPE_COMPONENT* GetComponentPtrAs()const;

	protected:
		Entity(bool mustUpdate);

		//void SetRenderInfo(RenderInfo*);

		virtual void Update() {}
		virtual void OnInitialize() {}
		virtual void OnShutdown() {}

	private:
		void ComponentUpdateEveryFrame(bool b, uint id);
		void OnAddComponent(Handle<Component>&);
		void UpdateComponents();

		void Initialize();
		void Shutdown();

		static uint s_idControl;

		Handle<Entity> m_hSelf;
		uint m_id;
		bool m_initialized;
		HashString m_name;
		bool m_mustUpdate;

		//SmartPtr<RenderInfo> m_pRenderInfo;
		//SimplePtr<CollisionInfo> m_pCollisionInfo;
		//PhysicsInfo m_physicsInfo;

		HandleManager<Component> m_handleComponents;
		Table<Handle<Component>, uint> m_tableComponents;
		Table<Handle<Component>, uint> m_tableComponentUpdates;
};

//=============================================================================
//	Entity template definition
//=============================================================================

////-------------------------------------------------------------------
////	GetRenderInfo
////-------------------------------------------------------------------
//template <class TYPE>
//inline TYPE* Entity::GetRenderInfoAs()
//{
//	if ( TYPE::GetTypeStatic() == m_pRenderInfo->GetType() )
//	{
//		return (TYPE*)m_pRenderInfo.Get();
//	}
//	else
//	{
//		ASSERT( "Type mismatch" );
//	}
//
//	return nullptr;
//}
////-------------------------------------------------------------------
//template <class TYPE>
//inline const TYPE* Entity::GetRenderInfoAs()const
//{
//	if ( TYPE::GetTypeStatic() == m_pRenderInfo->GetType() )
//	{
//		return (TYPE*)m_pRenderInfo.Get();
//	}
//	else
//	{
//		ASSERT( "Type mismatch" );
//	}
//
//	return nullptr;
//}
//-------------------------------------------------------------------
//	Convert
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* Entity::Convert()
{
	if ( GetClassRelationship<TYPE>() != eClassRelationship::None )
		return (TYPE*)this;
	ASSERT("Cannot convert entity");
	return nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE* Entity::Convert()const
{
	if ( GetClassRelationship<TYPE>() != eClassRelationship::None )
		return (const TYPE*)this;
	ASSERT("Cannot convert entity");
	return nullptr;
}
//-------------------------------------------------------------------
template <class ENTITY_TYPE>
inline ENTITY_TYPE* Entity::Convert(Handle<Entity>& hEntity)
{
	if ( !hEntity )
		return nullptr;
	return hEntity->Convert<ENTITY_TYPE>();
}
//-------------------------------------------------------------------
//	AddComponent
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
inline Handle<Component> Entity::AddComponent()
{
	for ( auto it = m_tableComponents.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship<COMPONENT_TYPE>() != eClassRelationship::None )
		{
			ASSERT( "Component type already included" );
			return Handle<Component>();
		}
	}

	Handle<Component> hNew = m_handleComponents.CreateAs<COMPONENT_TYPE>();
	OnAddComponent( hNew );

	return hNew;
}
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
inline HandleAs<Component, COMPONENT_TYPE> Entity::AddComponentAs()
{
	return HandleAs<Component, COMPONENT_TYPE>( AddComponent<COMPONENT_TYPE>() );
}
//-------------------------------------------------------------------
//	RemoveComponentByType
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
inline bool Entity::RemoveComponentByType()
{
	List<uint> ids;
	for ( auto it = m_tableComponents.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship<COMPONENT_TYPE>() != eClassRelationship::None )
			ids.AddLast( (*it)->GetId() );
	}

	CHECK( ids.Count() > 1 );

	for ( auto it = ids.GetIterator(); it; it++ )
		RemoveComponent( *it );
}
//-------------------------------------------------------------------
//	GetComponent
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
Handle<Component> Entity::GetComponent()
{
	for ( auto it = m_tableComponents.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship<COMPONENT_TYPE>() != eClassRelationship::None )
			return *it;
	}

	return Handle<Component>();
}
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
HandleAs<Component, COMPONENT_TYPE> Entity::GetComponentAs()
{
	Handle<Component> hResult = GetComponent<COMPONENT_TYPE>();

	return HandleAs<Component, COMPONENT_TYPE>( hResult );
}
//-------------------------------------------------------------------
//	GetComponentPtrAs
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
COMPONENT_TYPE* Entity::GetComponentPtrAs()
{
	for ( auto it = m_tableComponents.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship<COMPONENT_TYPE>() != eClassRelationship::None )
			return (COMPONENT_TYPE*)(*it).GetPtr();
	}

	return nullptr;
}
//-------------------------------------------------------------------
template <class COMPONENT_TYPE>
const COMPONENT_TYPE* Entity::GetComponentPtrAs()const
{
	for ( auto it = m_tableComponents.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship<COMPONENT_TYPE>() != eClassRelationship::None )
			return (COMPONENT_TYPE*)(*it).GetPtr();
	}

	return nullptr;
}
// TODO: delete this
////-------------------------------------------------------------------
////	UseCollision
////-------------------------------------------------------------------
//template <class TYPE>
//inline TYPE* Entity::UseCollision()
//{
//	if ( m_pCollisionInfo )
//	{
//		ASSERT( "Already has collision component" );
//		return nullptr;
//	}
//
//	m_pCollisionInfo = new TYPE;
//
//	SendCollisionAddEvent();
//
//	return (TYPE*)m_pCollisionInfo.Get();
//}
