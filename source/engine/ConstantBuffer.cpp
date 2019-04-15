#include "ConstantBuffer.h"
#include "../engine/CoreInterface.h"
#include "../engine/Pointer.h"

#ifdef DIRECTX11

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ConstantBufferApiImplementation::ConstantBufferApiImplementation()
{
	m_uiSizeOfBufferType = 0;
	m_initialized = false;
}
//-------------------------------------------------------------------
ConstantBufferApiImplementation::~ConstantBufferApiImplementation()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void ConstantBufferApiImplementation::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	platform::release( m_cbApi );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void ConstantBufferApiImplementation::Initialize(uint sizeOfBufferType)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	HRESULT hResult;
	D3D11_BUFFER_DESC constantBufferDesc;

	if ( sizeOfBufferType % 16 != 0 )
	{
		THROW( "Constant buffer size must be 16 bits aligned" );
	}

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeOfBufferType;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	hResult = g_pRenderer->GetApi().pDevice->CreateBuffer( &constantBufferDesc, nullptr, &m_cbApi.pConstantBuffer );
	if ( FAILED(hResult) )
	{
		THROW( "Failed creating constant buffer" );
	}

	m_uiSizeOfBufferType = sizeOfBufferType;

	m_initialized = true;
}
//-------------------------------------------------------------------
//	UseOnShader
//-------------------------------------------------------------------
void ConstantBufferApiImplementation::SetOnShader(uint slot, eShaderType shaderType, const void* pData)
{
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	hResult = g_pRenderer->GetApi().pDeviceContext->Map( m_cbApi.pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );

	auto dataPtr = (char*)mappedResource.pData;

	memoryCopy( (const char*)pData, dataPtr, m_uiSizeOfBufferType );

	g_pRenderer->GetApi().pDeviceContext->Unmap( m_cbApi.pConstantBuffer, 0 );

	if ( shaderType == eShaderType::VertexShader )
		g_pRenderer->GetApi().pDeviceContext->VSSetConstantBuffers( slot, 1, &m_cbApi.pConstantBuffer );
	else if ( shaderType == eShaderType::PixelShader )
		g_pRenderer->GetApi().pDeviceContext->PSSetConstantBuffers( slot, 1, &m_cbApi.pConstantBuffer );
	else
	{
		THROW( "Unknown shader type when attempting to bind constant buffer" );
	}
}

#endif