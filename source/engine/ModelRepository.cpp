#include "ModelRepository.h"
#include "../engine/Vertex.h"
#include "../engine/EngineGlobal.h"

/****************************************************************************************
	ModelVertex
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ModelRepository::ModelVertex::ModelVertex()
{
	positions = nullptr;
	texCoords = nullptr;
	normals = nullptr;
	colors = nullptr;
	indices = nullptr;
	tangents = nullptr;


	posCount = 0;
	texCoordCount = 0;
	normalCount = 0;
	colors = 0;
	indexCount = 0;
	tangentCount = 0;
}
//-------------------------------------------------------------------
ModelRepository::ModelVertex::~ModelVertex()
{
	safeArrayRelease( positions );
	safeArrayRelease( texCoords );
	safeArrayRelease( normals );
	safeArrayRelease( colors );
	safeArrayRelease( indices );
}

/****************************************************************************************
	ModelRepository
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ModelRepository::ModelRepository(uint key, HandleManager<Model>* pModelHandleManager) : m_tModelByLayouts( engineDefault::model::MODEL_REPOSITORY_TABLE_LAYOUT_SIZE, &tableHashFunc_Uint )
{
	CHECK( pModelHandleManager );
	m_key = key;
	m_pHandleMngr = pModelHandleManager;
	m_topology = eTopology::None;
	m_initialized = false;
}
//-------------------------------------------------------------------
ModelRepository::~ModelRepository()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void ModelRepository::Initialize()
{

	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void ModelRepository::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( auto it = m_tModelByLayouts.GetIterator(); it; it++ )
	{
		(*it).hModel.Get().Shutdown();
		m_pHandleMngr->Remove( (*it).hModel );
	}

	m_tModelByLayouts.Clear();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	SetModelIndex
//-------------------------------------------------------------------
void ModelRepository::SetModelIndex(const uint* indices, uint indexCount)
{
	CHECK( !indices || (indexCount > 0 && indices) );

	if ( indices && indexCount > 0 )
	{
		CHECK( m_vertex.indexCount == 0 );

		safeArrayRelease( m_vertex.indices );
		m_vertex.indices = new uint[indexCount];
		m_vertex.indexCount = indexCount;
		memoryCopy( indices, m_vertex.indices, indexCount );
	}
}
//-------------------------------------------------------------------
//	SetModelVertex_Pos
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_Pos(const math::Vec3* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.posCount == 0 );

	safeArrayRelease( m_vertex.positions );
	m_vertex.positions = new math::Vec3[count];

	m_vertex.posCount = count;
	memoryCopy( data, m_vertex.positions, count );
}
//-------------------------------------------------------------------
//	SetModelVertex_TexCoord
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_TexCoord(const math::Vec2* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.texCoordCount == 0 );

	safeArrayRelease( m_vertex.texCoords );
	m_vertex.texCoords = new math::Vec2[count];

	m_vertex.texCoordCount = count;
	memoryCopy( data, m_vertex.texCoords, count );
}
//-------------------------------------------------------------------
//	SetModelVertex_Normal
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_Normal(const math::Vec3* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.normalCount == 0 );

	safeArrayRelease( m_vertex.normals );
	m_vertex.normals = new math::Vec3[count];

	m_vertex.normalCount = count;
	memoryCopy( data, m_vertex.normals, count );
}
//-------------------------------------------------------------------
//	SetModelVertex_Tangent
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_Tangent(const math::Vec3* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.tangentCount == 0 );

	safeArrayRelease( m_vertex.tangents );
	m_vertex.tangents = new math::Vec3[count];

	m_vertex.tangentCount = count;
	memoryCopy( data, m_vertex.tangents, count );
}
//-------------------------------------------------------------------
//	SetModelVertex_Binormal
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_Binormal(const math::Vec3* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.binormalCount == 0 );

	safeArrayRelease( m_vertex.binormals );
	m_vertex.binormals = new math::Vec3[count];

	m_vertex.binormalCount = count;
	memoryCopy( data, m_vertex.binormals, count );
}
//-------------------------------------------------------------------
//	SetModelVertex_Color
//-------------------------------------------------------------------
void ModelRepository::SetModelVertex_Color(const math::Vec4* data, uint count)
{
	CHECK( count > 0 );
	CHECK( m_vertex.colorCount == 0 );

	safeArrayRelease( m_vertex.colors );
	m_vertex.colors = new math::Vec4[count];

	m_vertex.colorCount = count;
	memoryCopy( data, m_vertex.colors, count );
}
//-------------------------------------------------------------------
//	IsCompatible
//-------------------------------------------------------------------
bool ModelRepository::IsCompatible(const VertexLayout& layout)const
{
	if ( Vertex_P3::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0;

	if ( Vertex_P3C4::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0 && m_vertex.colorCount > 0;

	if ( Vertex_P3N3::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0 && m_vertex.normalCount > 0;

	if ( Vertex_P3T2::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0 && m_vertex.texCoordCount > 0;

	if ( Vertex_P3N3T2::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0 && m_vertex.normalCount > 0 && m_vertex.texCoordCount > 0;

	if ( Vertex_P3N3Tg3Bn3T2::GetLayout().IsEqual(layout) )
		return m_vertex.posCount > 0 && m_vertex.normalCount > 0 && m_vertex.tangentCount > 0 && m_vertex.binormalCount > 0 && m_vertex.texCoordCount > 0;

	THROW( "Layout not implemented" );
}
//-------------------------------------------------------------------
//	CreateForLayout
//-------------------------------------------------------------------
Handle<Model> ModelRepository::CreateForLayout(const VertexLayout& layout)
{
	if ( Vertex_P3::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3>();

	if ( Vertex_P3C4::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3C4>();

	if ( Vertex_P3N3::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3N3>();

	if ( Vertex_P3T2::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3T2>();

	if ( Vertex_P3N3T2::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3N3T2>();

	if ( Vertex_P3N3Tg3Bn3T2::GetLayout().IsEqual(layout) )
		return CreateForLayout<Vertex_P3N3Tg3Bn3T2>();

	THROW( "Layout not implemented" );

	return Handle<Model>();
}
//-------------------------------------------------------------------
//	CreateModelByLayoutForLayout
//-------------------------------------------------------------------
ModelRepository::ModelByVertexLayout* ModelRepository::CreateModelByLayoutForLayout(const VertexLayout& layout)
{
	ModelByVertexLayout* pModelByLayout = m_tModelByLayouts.Find( layout.id );

	if ( pModelByLayout )
		return pModelByLayout;

	m_tModelByLayouts.Add( ModelByVertexLayout(), layout.id );
	pModelByLayout = m_tModelByLayouts.Find( layout.id );
	pModelByLayout->pLayout = &layout;

	return pModelByLayout;
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec3 ModelRepository::GetPos(uint index)const
{
	if ( index < m_vertex.posCount )
		return m_vertex.positions[index];

	ASSERT( "Model layout does not have position" );
	return math::Vec3( 0.0f );
}
//-------------------------------------------------------------------
//	GetTexCoord
//-------------------------------------------------------------------
math::Vec2 ModelRepository::GetTexCoord(uint index)const
{
	if ( index < m_vertex.texCoordCount )
		return m_vertex.texCoords[index];

	ASSERT( "Model layout does not have texture coordinate" );
	return math::Vec2( 0.0f );
}
//-------------------------------------------------------------------
//	GetNormal
//-------------------------------------------------------------------
math::Vec3 ModelRepository::GetNormal(uint index)const
{
	if ( index < m_vertex.normalCount )
		return m_vertex.normals[index];

	ASSERT( "Model layout does not have normal" );
	return math::Vec3( 0.0f, 1.0f, 0.0f );
}
//-------------------------------------------------------------------
//	GetTangent
//-------------------------------------------------------------------
math::Vec3 ModelRepository::GetTangent(uint index)const
{
	if ( index < m_vertex.tangentCount )
		return m_vertex.tangents[index];

	ASSERT( "Model layout does not have tangent" );
	return math::Vec3( 0.0f, 1.0f, 0.0f );
}
//-------------------------------------------------------------------
//	GetBinormal
//-------------------------------------------------------------------
math::Vec3 ModelRepository::GetBinormal(uint index)const
{
	if ( index < m_vertex.binormalCount )
		return m_vertex.binormals[index];

	ASSERT( "Model layout does not have binormal" );
	return math::Vec3( 0.0f, 1.0f, 0.0f );
}
//-------------------------------------------------------------------
//	GetColor
//-------------------------------------------------------------------
math::Vec4 ModelRepository::GetColor(uint index)const
{
	if ( index < m_vertex.colorCount )
		return m_vertex.colors[index];

	ASSERT( "Model layout does not have color" );
	return math::Vec4( 1.0f );
}
