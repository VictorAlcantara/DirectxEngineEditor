#pragma once

#include "../engine/Model.h"
#include "../engine/Handle.h"
#include "../engine/Class.h"
#include "../engine/EngineGlobal.h"
#include "../engine/Table.h"

class ModelManager;

class ModelRepository : public NonCopyable
{
	friend class ModelManager;

	private:
		struct ModelByVertexLayout
		{
			const VertexLayout* pLayout;
			Handle<Model> hModel;

			ModelByVertexLayout()
			{
				pLayout = nullptr;
			}
		};

		struct ModelVertex
		{
			math::Vec3* positions;
			uint posCount;

			math::Vec2* texCoords;
			uint texCoordCount;

			math::Vec3* normals;
			uint normalCount;

			math::Vec3* tangents;
			uint tangentCount;

			math::Vec3* binormals;
			uint binormalCount;

			math::Vec4* colors;
			uint colorCount;

			uint* indices;
			uint indexCount;

			ModelVertex();
			~ModelVertex();
		};

	public:
		~ModelRepository();

		const String& GetModelFileName()const { return m_modelFileName; }

	private:
		ModelRepository(uint key, HandleManager<Model>*);

		void Initialize();
		void Shutdown();

		void SetModelFileName(const String& modelFileName) { m_modelFileName = modelFileName; }
		void SetModelTopology(eTopology t) { m_topology = t; }
		void SetModelIndex(const uint* indices, uint indexCount);

		void SetModelVertex_Pos(const math::Vec3*, uint);
		void SetModelVertex_TexCoord(const math::Vec2*, uint);
		void SetModelVertex_Normal(const math::Vec3*, uint);
		void SetModelVertex_Tangent(const math::Vec3*, uint);
		void SetModelVertex_Binormal(const math::Vec3*, uint);
		void SetModelVertex_Color(const math::Vec4*, uint);

		Handle<Model> CreateForLayout(const VertexLayout&);

		template <class VERTEX_TYPE>
		Handle<Model> CreateForLayout();

		template <class VERTEX_TYPE>
		void CreateVertices(VERTEX_TYPE*& outVertices, uint& outVertexCount);

		bool IsCompatible(const VertexLayout&)const;

		math::Vec3 GetPos(uint index)const;
		math::Vec2 GetTexCoord(uint index)const;
		math::Vec3 GetNormal(uint index)const;
		math::Vec3 GetTangent(uint index)const;
		math::Vec3 GetBinormal(uint index)const;
		math::Vec4 GetColor(uint index)const;

		ModelByVertexLayout* CreateModelByLayoutForLayout(const VertexLayout& layout); // WHAT THE HELL IS THIS NAME???

		template <class VERTEX_TYPE>
		Handle<Model> InitializeModelForLayout(ModelByVertexLayout& modelByLayout, const VERTEX_TYPE* vertices, uint vertexCount);

		bool m_initialized;
		String m_modelFileName;
		uint m_key;
		eTopology m_topology;
		ModelVertex m_vertex;
		HandleManager<Model>* m_pHandleMngr;

		Table<ModelByVertexLayout, uint> m_tModelByLayouts;
};

//-------------------------------------------------------------------
//	CreateForLayout_Generic
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline Handle<Model> ModelRepository::CreateForLayout()
{
	ModelByVertexLayout* pModelByVertexLayout = CreateModelByLayoutForLayout(VERTEX_TYPE::GetLayout());

	if ( pModelByVertexLayout->hModel.IsValid() )
		return pModelByVertexLayout->hModel;

	uint vertexCount = 0;
	VERTEX_TYPE* vertices = nullptr;
	CreateVertices<VERTEX_TYPE>( vertices, vertexCount );

	pModelByVertexLayout->hModel = InitializeModelForLayout( *pModelByVertexLayout, vertices, vertexCount );

	safeArrayRelease( vertices );

	return pModelByVertexLayout->hModel;
}
//-------------------------------------------------------------------
//	InitializeModelForLayout
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
Handle<Model> ModelRepository::InitializeModelForLayout(ModelByVertexLayout& modelByLayout, const VERTEX_TYPE* vertices, uint vertexCount)
{
	CHECK( !modelByLayout.hModel.IsValid() );

	modelByLayout.hModel = m_pHandleMngr->Create();
	modelByLayout.hModel->Initialize( m_topology, vertices, vertexCount, m_vertex.indices, m_vertex.indexCount );
	modelByLayout.hModel->m_id = m_key;

	return modelByLayout.hModel;
}
//-------------------------------------------------------------------
//	CreateVertices
//-------------------------------------------------------------------
template <class VERTEX_TYPE>
inline void ModelRepository::CreateVertices(VERTEX_TYPE*& outVertices, uint& outVertexCount)
{
	static_assert( "Not implemented" );
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3>(Vertex_P3*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
		outVertices[i].pos = GetPos(i);
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3C4>(Vertex_P3C4*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3C4[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
	{
		outVertices[i].pos = GetPos(i);
		outVertices[i].color = GetColor(i);
	}
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3N3>(Vertex_P3N3*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3N3[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
	{
		outVertices[i].pos = GetPos(i);
		outVertices[i].normal = GetNormal(i);
	}
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3T2>(Vertex_P3T2*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3T2[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
	{
		outVertices[i].pos = GetPos(i);
		outVertices[i].texCoord = GetTexCoord(i);
	}
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3N3T2>(Vertex_P3N3T2*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3N3T2[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
	{
		outVertices[i].pos = GetPos(i);
		outVertices[i].normal = GetNormal(i);
		outVertices[i].texCoord = GetTexCoord(i);
	}
}
//-------------------------------------------------------------------
template <>
inline void ModelRepository::CreateVertices<Vertex_P3N3Tg3Bn3T2>(Vertex_P3N3Tg3Bn3T2*& outVertices, uint& outVertexCount)
{
	outVertexCount = m_vertex.posCount;
	outVertices = new Vertex_P3N3Tg3Bn3T2[outVertexCount];

	for ( uint i = 0; i < outVertexCount; i++ )
	{
		outVertices[i].pos = GetPos(i);
		outVertices[i].normal = GetNormal(i);
		outVertices[i].tangent = GetTangent(i);
		outVertices[i].binormal = GetBinormal(i);
		outVertices[i].texCoord = GetTexCoord(i);
	}
}
