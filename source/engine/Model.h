#pragma once

#include "Type.h"
#include "../engine/Class.h"
#include "../engine/Math.h"
#include "Platform.h"
#include "../engine/Vertex.h"
#include "RenderUtil.h"

class Model : public NonCopyable
{
	friend class ModelRepository;

	public:
		Model();
		~Model();

		template <class VERTEX_TYPE>
		void Initialize(eTopology topology, const VERTEX_TYPE* pVertices, uint vertexCount, uint* pIndices, uint indexCount);

		template <class VERTEX_TYPE>
		void Initialize(eTopology topology, const VERTEX_TYPE* pVertices, uint vertexCount);
		
		void Shutdown();

		uint GetIndexCount()const { return m_indexCount; }
		uint GetVertexCount()const { return m_vertexCount; }
		uint GetVertexTypeSize()const { return m_vertexTypeSize; }
		const VertexLayout& GetVertexLayout()const { return *m_pVertexLayout; }
		bool UsesIndex()const { return m_usesIndex; }

		uint GetId()const { return m_id; }
		bool IsInitialized()const { return m_initialized; }

		const math::Aabb3& GetAabb()const;

		platform::ModelApi& GetApi();

	private:
		void Initialize(eTopology topology, const VertexLayout* pVertexLayout, const void* pVertices, uint vertexTypeSize, uint vertexCount, uint* pIndices, uint indexCount);
		template <class VERTEX_TYPE>
		void CalculateAabb(const VERTEX_TYPE* pVertices, uint vertexCount);

		uint m_id;
		bool m_initialized;
		uint m_vertexCount;
		uint m_indexCount;
		bool m_usesIndex;
		uint m_vertexTypeSize;
		eTopology m_topology;
		platform::ModelApi m_modelApi;
		const VertexLayout* m_pVertexLayout;
		math::Aabb3 m_aabb;
};

//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline void Model::Initialize(eTopology topology, const VERTEX_TYPE* pVertices, uint vertexCount, uint* pIndices, uint indexCount)
{
	Initialize( topology, &VERTEX_TYPE::GetLayout(), (const void*)pVertices, VERTEX_TYPE::GetLayout().size, vertexCount, pIndices, indexCount );
	CalculateAabb(pVertices, vertexCount);
}
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline void Model::Initialize(eTopology topology, const VERTEX_TYPE* pVertices, uint vertexCount)
{
	Initialize( topology, &VERTEX_TYPE::GetLayout(), (const void*)pVertices, VERTEX_TYPE::GetLayout().size, vertexCount, nullptr, 0 );
	CalculateAabb(pVertices, vertexCount);
}
//-------------------------------------------------------------------
//	CalculateAabb
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline void Model::CalculateAabb(const VERTEX_TYPE* pVertices, uint vertexCount)
{
	if ( vertexCount == 0 )
		return;

	CHECK( IsInitialized() );

	m_aabb.min = pVertices[0].pos;
	m_aabb.max = pVertices[0].pos;

	for ( uint i = 1; i < vertexCount; i++ )
	{
		if ( pVertices[i].pos.x < m_aabb.min.x ) m_aabb.min.x = pVertices[i].pos.x;
		if ( pVertices[i].pos.y < m_aabb.min.y ) m_aabb.min.y = pVertices[i].pos.y;
		if ( pVertices[i].pos.z < m_aabb.min.z ) m_aabb.min.z = pVertices[i].pos.z;

		if ( pVertices[i].pos.x > m_aabb.max.x ) m_aabb.max.x = pVertices[i].pos.x;
		if ( pVertices[i].pos.y > m_aabb.max.y ) m_aabb.max.y = pVertices[i].pos.y;
		if ( pVertices[i].pos.z > m_aabb.max.z ) m_aabb.max.z = pVertices[i].pos.z;
	}
}