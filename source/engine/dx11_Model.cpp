#include "api_Model.h"

#ifdef DIRECTX11

#include "../engine/CoreInterface.h"
#include "../engine/Exception.h"

//=============================================================================
//	Helper function declaration
//=============================================================================

D3D11_PRIMITIVE_TOPOLOGY toDxTopology(eTopology topology);

//=============================================================================
//	Platform function definition
//=============================================================================

//-------------------------------------------------------------------
//	initializeModel
//-------------------------------------------------------------------
platform::model::ModelInitializationResult platform::model::initializeModel(ModelApi& api, eTopology topology, const void* pVertices, uint vertexTypeSize, uint vertexCount, uint* pIndices, uint indexCount)
{
	ModelInitializationResult result;

	api.topology = toDxTopology( topology );
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hResult;

	if ( topology == eTopology::None )
	{
		THROW( "platform::model::initializeModel() no topology informed" );
	}

	result.vertexCount = vertexCount;
	result.indexCount = indexCount;
	result.vertexTypeSize = vertexTypeSize;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = result.vertexTypeSize * result.vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hResult = g_pRenderer->GetApi().pDevice->CreateBuffer( &vertexBufferDesc, &vertexData, &api.pVertexBuffer );
	if ( FAILED(hResult) )
		THROW( "Failed creating vertex buffer for model" );

	if ( pIndices && indexCount > 0 )
	{
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(uint) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = pIndices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		hResult = g_pRenderer->GetApi().pDevice->CreateBuffer( &indexBufferDesc, &indexData, &api.pIndexBuffer );
		if ( FAILED(hResult) )
			THROW( "Failed creating index buffer for model" );

		result.usesIndex = true;
	}

	result.initialized = true;

	return result;
}

//=============================================================================
//	Helper function definition
//=============================================================================

//-------------------------------------------------------------------
//	toDxTopology
//-------------------------------------------------------------------
D3D11_PRIMITIVE_TOPOLOGY toDxTopology(eTopology topology)
{
	switch ( topology )
	{
		case eTopology::Point: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case eTopology::Line: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case eTopology::LineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case eTopology::Triangle: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case eTopology::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			THROW( "toDxTopology() unrecognized topology" );
	}
}



#endif

