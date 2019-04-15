#include "Partition.h"
#include "../engine/Component_Render.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"
#include "../engine/TableHashFunction.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	Process_PartitionWaitForData
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_PartitionWaitForData::Process_PartitionWaitForData(PartitionScene* pOwner)
{
	CHECK( pOwner );
	m_pOwner = pOwner;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_PartitionWaitForData::Update()
{
	for ( auto pNode = m_waitQueue.GetFirst(); pNode; )
	{
		auto pNext = pNode->GetNext();

		if ( pNode->GetValue().component->GetData() )
		{
			m_pOwner->OnChange( pNode->GetValue().component );
			pNode->Remove();
		}
		else
		{
			pNode->GetValue().checkCount++;
			CHECK( pNode->GetValue().checkCount < 1000 );
		}

		pNode = pNext;
	}
}
//-------------------------------------------------------------------
//	AddToWaitQueue
//-------------------------------------------------------------------
void Process_PartitionWaitForData::AddToWaitQueue(HandleAs<Component, Component_Render>& hObject)
{
	#ifdef DEBUG_MODE
		// Repetition should not occur by design of how entities are added to the partition (and, consequentially, to the wait queue). But just to check...
		for ( auto it = m_waitQueue.GetIterator(); it; it++ )
		{
			CHECK( hObject->GetId() != (*it).component->GetId() );
		}
		CHECK( !hObject->GetData() );
	#endif

	m_waitQueue.AddLast( WaitData(hObject) );
}

/****************************************************************************************
	PartitionBlock
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
PartitionScene::PartitionBlock::PartitionBlock(const math::Vec3& pos, const math::Vec3& scale) : components(1000, tableHashFunc_Uint)
{
	aabb = math::Aabb3(scale) + pos;
}

/****************************************************************************************
	PartitionScene
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
PartitionScene::PartitionScene() : m_outsideBlocks(5000, tableHashFunc_Uint)
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void PartitionScene::OnShutdown()
{
	Clear();
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void PartitionScene::OnInitialize()
{
	m_pProcessWaitForData = new Process_PartitionWaitForData(this);
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void PartitionScene::Clear()
{
	if ( m_pProcessWaitForData->GetState() == eProcessState::Running )
		m_pProcessWaitForData->Exit();

	m_blocks.Clear();
	m_outsideBlocks.Clear();
}
//-------------------------------------------------------------------
//	GetFromFrustum
//-------------------------------------------------------------------
void PartitionScene::GetFromFrustum(const math::Frustum& frustum, const math::Aabb3& frustumAabb, Table<HandleAs<Component, Component_Render>, uint>& result)
{
	#ifdef DEBUG_MODE
		uint outsideCount = m_outsideBlocks.Count();
		uint blockCount = m_blocks.Count();
		if ( outsideCount > 20 && blockCount > 0 )
		{
			for ( auto it = m_outsideBlocks.GetIterator(); it; it++ )
			{
				debugToOutput( String( "Type:  " + (*it)->GetOwner()->GetClassTag().GetString() ).AsChar() );
				debugToOutput( String( "Pos:   " + string::vec3ToStr((*it)->GetData()->GetTransform().GetPos())).AsChar() );
				debugToOutput( String( "Scale: " + string::vec3ToStr((*it)->GetData()->GetAabbGlobal().GetScale())).AsChar() );
			}

			ASSERT( "There are more than 20 entities outside of the blocks" );
		}
	#endif

	for ( auto it = m_blocks.GetIterator(); it; it++ )
	{
		auto pBlock = *it;
		if ( math::intersectAabb3( pBlock->aabb, frustumAabb ).collision && math::intersectAabb3Frustum(pBlock->aabb, frustum) )
		{
			for ( auto itBlockContent = pBlock->components.GetIterator(); itBlockContent; itBlockContent++ )
				result.AddIfNotExists( *itBlockContent, (*itBlockContent)->GetId() );
		}
	}

	for ( auto it = m_outsideBlocks.GetIterator(); it; it++ )
		result.AddIfNotExists( *it, (*it)->GetId() );
}
//-------------------------------------------------------------------
//	GetFromAabb
//-------------------------------------------------------------------
void PartitionScene::GetFromAabb(const math::Aabb3& aabb, Table<HandleAs<Component, Component_Render>, uint>& result, bool compareToEntityAabb)
{
	for ( auto it = m_blocks.GetIterator(); it; it++ )
	{
		auto pBlock = *it;
		if ( math::intersectAabb3(pBlock->aabb, aabb).collision )
		{
			for ( auto itBlockContent = pBlock->components.GetIterator(); itBlockContent; itBlockContent++ )
				AddToTable( compareToEntityAabb, aabb, result, *itBlockContent );
		}
	}

	for ( auto it = m_outsideBlocks.GetIterator(); it; it++ )
		AddToTable( compareToEntityAabb, aabb, result, *it );
}
//-------------------------------------------------------------------
//	AddBlock
//-------------------------------------------------------------------
void PartitionScene::AddBlock(const math::Vec3& pos, const math::Vec3& scale)
{
	CHECK( math::min(scale) > 0.0f );

	SmartPtr<PartitionBlock> pBlock = new PartitionBlock(pos, scale);
	math::Aabb3 adjustedAabb = math::Aabb3(scale * 1.1f) + pos;
	m_blocks.AddLast( pBlock );

	Table<HandleAs<Component, Component_Render>, uint> intersection( 500, tableHashFunc_Uint );
	GetFromAabb( adjustedAabb, intersection, false );

	for ( auto it = intersection.GetIterator(); it; it++ )
	{
		uint componentId = (*it)->GetId();
		pBlock->components.AddIfNotExists( *it, componentId );
		m_outsideBlocks.Remove( componentId );
	}
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void PartitionScene::Add(HandleAs<Component, Component_Render>& hObject)
{
	if ( !hObject->GetData() )
	{
		if ( m_pProcessWaitForData->GetState() != eProcessState::Running )
		{
			g_pProcess->Add( m_pProcessWaitForData.As<Process>() );
			m_pProcessWaitForData->AddToWaitQueue( hObject );
		}
	}
	else
	{
		#ifdef DEBUG_MODE
			math::Vec3 pos = hObject->GetData()->GetTransform().GetPos();
			math::Vec3 scale = hObject->GetData()->GetAabbGlobal().GetScale();
			String type = hObject->GetOwner()->GetClassTag().GetString();
			uint idEntity = hObject->GetOwner()->GetId();
			uint idComponent = hObject->GetId();
		#endif

		bool hasAddedToPartition = false;
		for ( auto itBlock = m_blocks.GetIterator(); itBlock; itBlock++ )
		{
			auto pBlock = *itBlock;
			if ( math::intersectAabb3(pBlock->aabb, hObject->GetData()->GetAabbGlobal()).collision )
			{
				pBlock->components.AddIfNotExists( hObject, hObject->GetId() );
				hasAddedToPartition = true;
			}
		}

		if ( !hasAddedToPartition )
			m_outsideBlocks.Add( hObject, hObject->GetId() );

		#ifdef DEBUG_MODE
			// Should never be true, but just to check for some possible problem...
			if ( hasAddedToPartition )
				CHECK( !m_outsideBlocks.Find(hObject->GetId()) );
		#endif
	}
}
//-------------------------------------------------------------------
//	OnChange
//-------------------------------------------------------------------
void PartitionScene::OnChange(HandleAs<Component, Component_Render>& hObject)
{
	Remove( hObject.GetBase() );
	Add( hObject );
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void PartitionScene::Remove(Handle<Component>& hObject)
{
	Remove( hObject->GetId() );
}
//-------------------------------------------------------------------
void PartitionScene::Remove(uint componentId)
{
	for ( auto it = m_blocks.GetIterator(); it; it++ )
		(*it)->components.Remove( componentId );
	m_outsideBlocks.Remove( componentId );
}
//-------------------------------------------------------------------
//	AddToTable
//-------------------------------------------------------------------
void PartitionScene::AddToTable(bool compareToEntityAabb, const math::Aabb3& aabb, Table<HandleAs<Component, Component_Render>, uint>& outResult, HandleAs<Component, Component_Render>& hCmpRender)
{
	if ( !compareToEntityAabb || math::intersectAabb3(aabb, hCmpRender->GetData()->GetAabbGlobal()).collision )
		outResult.AddIfNotExists( hCmpRender, hCmpRender->GetId() );
}

/****************************************************************************************
	Process_PartitionWaitCollision
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_PartitionWaitCollision::Process_PartitionWaitCollision(PartitionCollision* pOwner)
{
	CHECK( pOwner );
	m_pOwner = pOwner;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_PartitionWaitCollision::Update()
{
	for ( auto pNode = m_waitQueue.GetFirst(); pNode; )
	{
		auto pNext = pNode->GetNext();

		if ( pNode->GetValue().component->GetOwner()->GetComponentPtrAs<Component_Physics>() || 
			pNode->GetValue().component->GetOwner()->GetComponentPtrAs<Component_Render>() || 
			pNode->GetValue().component->HasCollider() )
		{
			m_pOwner->OnChange( pNode->GetValue().component );
			pNode->Remove();
		}
		else
		{
			pNode->GetValue().checkCount++;
			CHECK( pNode->GetValue().checkCount < 1000 );
		}

		pNode = pNext;
	}
}
//-------------------------------------------------------------------
//	AddToWaitQueue
//-------------------------------------------------------------------
void Process_PartitionWaitCollision::AddToWaitQueue(HandleAs<Component, Component_Collision>& hObject)
{
	#ifdef DEBUG_MODE
		// Repetition should not occur by design of how entities are added to the partition (and, consequentially, to the wait queue). But just to check...
		for ( auto it = m_waitQueue.GetIterator(); it; it++ )
		{
			CHECK( hObject->GetId() != (*it).component->GetId() );
		}
		CHECK( !hObject->GetOwner()->GetComponentPtrAs<Component_Physics>() );
	#endif

	m_waitQueue.AddLast( WaitData(hObject) );
}

/****************************************************************************************
	PartitionBlock
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
PartitionCollision::PartitionBlock::PartitionBlock(const math::Vec3& pos, const math::Vec3& scale) : components(1000, tableHashFunc_Uint)
{
	aabb = math::Aabb3(scale) + pos;
	debugAddEntity = false;
}

/****************************************************************************************
	PartitionScene
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
PartitionCollision::PartitionCollision() : m_outsideBlocks(5000, tableHashFunc_Uint)
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void PartitionCollision::OnShutdown()
{
	Clear();
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void PartitionCollision::OnInitialize()
{
	m_pProcessWait = new Process_PartitionWaitCollision(this);
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void PartitionCollision::Clear()
{
	if ( m_pProcessWait->GetState() == eProcessState::Running )
		m_pProcessWait->Exit();

	m_blocks.Clear();
	m_outsideBlocks.Clear();
}
//-------------------------------------------------------------------
//	GetFromAabb
//-------------------------------------------------------------------
void PartitionCollision::GetFromAabb(const math::Aabb3& aabb, Table<HandleAs<Component, Component_Collision>, uint>& result, bool compareToEntityAabb)
{
	#ifdef DEBUG_MODE
		uint outsideCount = m_outsideBlocks.Count();
		uint blockCount = m_blocks.Count();
		if ( outsideCount > 20 && blockCount > 0 )
		{
			ASSERT( "There are more than 20 entities outside of the blocks" );
		}
	#endif

	GetFromAabb_NoOutsideCheck( aabb, result, compareToEntityAabb );
}
//-------------------------------------------------------------------
void PartitionCollision::GetFromAabb_NoOutsideCheck(const math::Aabb3& aabb, Table<HandleAs<Component, Component_Collision>, uint>& result, bool compareToEntityAabb)
{
	for ( auto it = m_blocks.GetIterator(); it; it++ )
	{
		auto pBlock = *it;
		if ( math::intersectAabb3(pBlock->aabb, aabb).collision )
		{
			for ( auto itBlockContent = pBlock->components.GetIterator(); itBlockContent; itBlockContent++ )
			{
				#ifdef DEBUG_MODE
					CHECK( !pBlock->debugEntityContact.IsValid() || pBlock->debugEntityContact != (*itBlockContent)->GetOwner()->GetClassTag() );
				#endif

				AddToTable( compareToEntityAabb, aabb, result, *itBlockContent );
			}
		}
	}

	for ( auto it = m_outsideBlocks.GetIterator(); it; it++ )
		AddToTable( compareToEntityAabb, aabb, result, *it );
}
//-------------------------------------------------------------------
//	AddBlock
//-------------------------------------------------------------------
void PartitionCollision::AddBlock(const math::Vec3& pos, const math::Vec3& scale, bool debugAddEntity, const String& debugEntityContact)
{
	CHECK( math::min(scale) > 0.0f );

	SmartPtr<PartitionBlock> pBlock = new PartitionBlock(pos, scale);
	pBlock->debugAddEntity = debugAddEntity;
	pBlock->debugEntityContact = debugEntityContact;
	math::Aabb3 adjustedAabb = math::Aabb3(scale * 1.1f) + pos;
	m_blocks.AddLast( pBlock );

	Table<HandleAs<Component, Component_Collision>, uint> intersection( 500, tableHashFunc_Uint );
	GetFromAabb_NoOutsideCheck( adjustedAabb, intersection, true );

	#ifdef DEBUG_MODE
		CHECK( !debugAddEntity || intersection.Count() == 0 );
	#endif

	for ( auto it = intersection.GetIterator(); it; it++ )
	{
		if ( math::intersectAabb3(pBlock->aabb, (*it)->GetAabbGlobal()).collision )
		{
			uint componentId = (*it)->GetId();
			pBlock->components.AddIfNotExists( *it, componentId );
			m_outsideBlocks.Remove( componentId );
		}
	}
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void PartitionCollision::Add(HandleAs<Component, Component_Collision>& hObject)
{
	const String& classTag = hObject->GetOwner()->GetClassTag().GetString();
	bool hasCollider = hObject->HasCollider();
	if ( !hasCollider || (!hObject->GetOwner()->GetComponentPtrAs<Component_Physics>() && !hObject->GetOwner()->GetComponentPtrAs<Component_Render>()) )
	{
		if ( m_pProcessWait->GetState() != eProcessState::Running )
		{
			g_pProcess->Add( m_pProcessWait.As<Process>() );
			m_pProcessWait->AddToWaitQueue( hObject );
		}
	}
	else
	{
		bool hasAddedToPartition = false;
		for ( auto itBlock = m_blocks.GetIterator(); itBlock; itBlock++ )
		{
			auto pBlock = *itBlock;
			if ( math::intersectAabb3(pBlock->aabb, hObject->GetAabbGlobal()).collision )
			{
				#ifdef DEBUG_MODE
					CHECK( !pBlock->debugAddEntity );
				#endif

				pBlock->components.AddIfNotExists( hObject, hObject->GetId() );
				hasAddedToPartition = true;
			}
		}

		if ( !hasAddedToPartition )
			m_outsideBlocks.Add( hObject, hObject->GetId() );

		#ifdef DEBUG_MODE
			// Should never be true, but just to check for some possible problem...
			if ( hasAddedToPartition )
				CHECK( !m_outsideBlocks.Find(hObject->GetId()) );
		#endif
	}
}
//-------------------------------------------------------------------
//	OnChange
//-------------------------------------------------------------------
void PartitionCollision::OnChange(HandleAs<Component, Component_Collision>& hObject)
{
	Remove( hObject.GetBase() );
	Add( hObject );
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void PartitionCollision::Remove(Handle<Component>& hObject)
{
	Remove( hObject->GetId() );
}
//-------------------------------------------------------------------
void PartitionCollision::Remove(uint componentId)
{
	for ( auto it = m_blocks.GetIterator(); it; it++ )
		(*it)->components.Remove( componentId );
	m_outsideBlocks.Remove( componentId );
}
//-------------------------------------------------------------------
//	AddToTable
//-------------------------------------------------------------------
void PartitionCollision::AddToTable(bool compareToEntityAabb, const math::Aabb3& aabb, Table<HandleAs<Component, Component_Collision>, uint>& outResult, HandleAs<Component, Component_Collision>& hCmpCollision)
{
	if ( !compareToEntityAabb || math::intersectAabb3(aabb, hCmpCollision->GetAabbGlobal()).collision )
		outResult.AddIfNotExists( hCmpCollision, hCmpCollision->GetId() );
}