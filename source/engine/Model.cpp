#include "../engine/Model.h"
#include "../engine/Exception.h"
#include "../engine/Pointer.h"
#include "../engine/CoreInterface.h"
#include "api_Model.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Model::Model()
{
	m_vertexTypeSize = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_topology = eTopology::None;
	m_pVertexLayout = nullptr;
	m_usesIndex = false;
	m_id = 0;
	m_initialized = false;
}
//-------------------------------------------------------------------
Model::~Model()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Model::Initialize(eTopology topology, const VertexLayout* pVertexLayout, const void* pVertices, uint vertexTypeSize, uint vertexCount, uint* pIndices, uint indexCount)
{
	if ( IsInitialized() )
	{
		ASSERT( "Model already initialized" );
		return;
	}

	auto result = platform::model::initializeModel( m_modelApi, topology, pVertices, vertexTypeSize, vertexCount, pIndices, indexCount );

	if ( result.initialized )
	{
		m_topology = topology;
		m_pVertexLayout = pVertexLayout;
		m_usesIndex = result.usesIndex;
		m_vertexCount = result.vertexCount;
		m_indexCount = result.indexCount;
		m_vertexTypeSize = result.vertexTypeSize;
		m_initialized = true;
	}
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Model::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	platform::release( m_modelApi );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	GetAabb
//-------------------------------------------------------------------
const math::Aabb3& Model::GetAabb()const
{
	CHECK(math::lengthSq(m_aabb.GetScale()) > 0.0f); 
	return m_aabb;
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::ModelApi& Model::GetApi()
{
	return m_modelApi;
}
