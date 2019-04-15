#pragma once

#include "../engine/Class.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Table.h"
#include "../engine/Process.h"

/*
	TODO: 
		- unify partition classes
		- use a different data structure to get objects inside partition blocks (binary tree maybe?)
*/

class Component;
class Component_Render;
class Component_Collision;
class PartitionScene;
class PartitionCollision;

/****************************************************************************************
	Process_PartitionWaitForData
*****************************************************************************************/

class Process_PartitionWaitForData : public Process
{
	private:
		struct WaitData
		{
			HandleAs<Component, Component_Render> component;
			uint checkCount;

			WaitData(HandleAs<Component, Component_Render>& _component) : component(_component), checkCount(0) {}
			WaitData() : checkCount(0) {}
		};

	public:
		Process_PartitionWaitForData(PartitionScene* pOwner);
		virtual ~Process_PartitionWaitForData() = default;

		void AddToWaitQueue(HandleAs<Component, Component_Render>&);

	protected:
		void Update()override final;

	private:
		PartitionScene* m_pOwner;
		List<WaitData> m_waitQueue;
};

/****************************************************************************************
	PartitionScene
*****************************************************************************************/

class PartitionScene : public NonCopyable, public PublicModule
{
	struct PartitionBlock
	{
		Table<HandleAs<Component, Component_Render>, uint> components;
		math::Aabb3 aabb;

		PartitionBlock(const math::Vec3& pos, const math::Vec3& scale);
		PartitionBlock(const PartitionBlock&) = delete;
		PartitionBlock& operator=(const PartitionBlock&) = delete;
	};

	public:
		PartitionScene();
		virtual ~PartitionScene() = default;

		void AddBlock(const math::Vec3& pos, const math::Vec3& scale);
		void Clear();

		void Add(HandleAs<Component, Component_Render>&);
		void OnChange(HandleAs<Component, Component_Render>&);
		void Remove(Handle<Component>&);
		void Remove(uint);

		void GetFromFrustum(const math::Frustum&, const math::Aabb3& frustumAabb, Table<HandleAs<Component, Component_Render>, uint>& outResult);
		void GetFromAabb(const math::Aabb3&, Table<HandleAs<Component, Component_Render>, uint>& result, bool compareEntityToAabb);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void AddToTable(bool compareToEntityAabb, const math::Aabb3&, Table<HandleAs<Component, Component_Render>, uint>& outResult, HandleAs<Component, Component_Render>&);

		List<SmartPtr<PartitionBlock>> m_blocks;
		Table<HandleAs<Component, Component_Render>, uint> m_outsideBlocks;
		SmartPtr<Process_PartitionWaitForData> m_pProcessWaitForData;
};

/****************************************************************************************
	Process_PartitionWaitCollision
*****************************************************************************************/

class Process_PartitionWaitCollision : public Process
{
	private:
		struct WaitData
		{
			HandleAs<Component, Component_Collision> component;
			uint checkCount;

			WaitData(HandleAs<Component, Component_Collision>& _component) : component(_component), checkCount(0) {}
			WaitData() : checkCount(0) {}
		};

	public:
		Process_PartitionWaitCollision(PartitionCollision* pOwner);
		virtual ~Process_PartitionWaitCollision() = default;

		void AddToWaitQueue(HandleAs<Component, Component_Collision>&);

	protected:
		void Update()override final;

	private:
		PartitionCollision* m_pOwner;
		List<WaitData> m_waitQueue;
};

/****************************************************************************************
	PartitionCollision
*****************************************************************************************/

class PartitionCollision : public NonCopyable, public PublicModule
{
	struct PartitionBlock
	{
		Table<HandleAs<Component, Component_Collision>, uint> components;
		math::Aabb3 aabb;
		HashString debugEntityContact;
		bool debugAddEntity;

		PartitionBlock(const math::Vec3& pos, const math::Vec3& scale);
		PartitionBlock(const PartitionBlock&) = delete;
		PartitionBlock& operator=(const PartitionBlock&) = delete;
	};

	public:
		PartitionCollision();
		virtual ~PartitionCollision() = default;

		void AddBlock(const math::Vec3& pos, const math::Vec3& scale, bool debugAddEntity, const String& debugEntityContact);
		void Clear();

		void Add(HandleAs<Component, Component_Collision>&);
		void OnChange(HandleAs<Component, Component_Collision>&);
		void Remove(Handle<Component>&);
		void Remove(uint);

		void GetFromAabb(const math::Aabb3&, Table<HandleAs<Component, Component_Collision>, uint>& result, bool compareToEntityAabb);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void GetFromAabb_NoOutsideCheck(const math::Aabb3&, Table<HandleAs<Component, Component_Collision>, uint>& result, bool compareToEntityAabb);
		void AddToTable(bool compareToEntityAabb, const math::Aabb3&, Table<HandleAs<Component, Component_Collision>, uint>& outResult, HandleAs<Component, Component_Collision>&);

		List<SmartPtr<PartitionBlock>> m_blocks;
		Table<HandleAs<Component, Component_Collision>, uint> m_outsideBlocks;
		SmartPtr<Process_PartitionWaitCollision> m_pProcessWait;
};
