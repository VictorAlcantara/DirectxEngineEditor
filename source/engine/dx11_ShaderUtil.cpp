#include "api_ShaderUtil.h"

#ifdef DIRECTX11

#include "../engine/String.h"
#include "../engine/CoreInterface.h"

//=============================================================================
//	Helper function declaration
//=============================================================================

void compileShader(const wchar* file, const char* shaderTypeVersion, ID3DBlob** ppByteCodeOut);
DXGI_FORMAT dx11Format(const VertexLayoutElement& element);
const char* dx11SemanticName(const VertexLayoutElement& element);
D3D11_INPUT_ELEMENT_DESC convertToDx11Layout(const VertexLayoutElement& element);
Array<D3D11_INPUT_ELEMENT_DESC> createLayoutFromVertex(const Array<VertexLayoutElement>& elements);

//=============================================================================
//	Platform function definition
//=============================================================================

//-------------------------------------------------------------------
//	createVertexShaderAndLayout
//-------------------------------------------------------------------
void platform::shader::createVertexShaderAndLayout(const wchar* file, RendererApi& rendererApi, ShaderApi& shaderApi, const VertexLayout* pLayout)
{
	ID3DBlob* pVertexShaderByteCode = nullptr;

	compileShader( file, "vs_5_0", &pVertexShaderByteCode );

	HRESULT hResult = rendererApi.pDevice->CreateVertexShader( pVertexShaderByteCode->GetBufferPointer(), pVertexShaderByteCode->GetBufferSize(), 
		nullptr, &shaderApi.pVertexShader );

	if ( FAILED(hResult) )
	{
		THROW( "Failed creating vertex shader" );
	}

	auto polygonLayout = createLayoutFromVertex(pLayout->elements);

	hResult = rendererApi.pDevice->CreateInputLayout( polygonLayout.GetPointer(), polygonLayout.Count(), 
		pVertexShaderByteCode->GetBufferPointer(), pVertexShaderByteCode->GetBufferSize(), &shaderApi.pLayout );

	if ( FAILED(hResult) )
	{
		THROW( "Failed creating input layout for vertex shader" );
	}

	safeComRelease( pVertexShaderByteCode );
}
//-------------------------------------------------------------------
//	createPixelShaderAndLayout
//-------------------------------------------------------------------
void platform::shader::createPixelShader(const wchar* file, RendererApi& rendererApi, ShaderApi& shaderApi)
{
	ID3DBlob* pPixelShaderByteCode = nullptr;

	compileShader( file, "ps_5_0", &pPixelShaderByteCode );

	HRESULT hResult = rendererApi.pDevice->CreatePixelShader( pPixelShaderByteCode->GetBufferPointer(), pPixelShaderByteCode->GetBufferSize(),
		nullptr, &shaderApi.pPixelShader );

	if ( FAILED(hResult) )
	{
		THROW( "Failed creating pixel shader" );
	}

	safeComRelease( pPixelShaderByteCode );
}
//-------------------------------------------------------------------
//	vertexShaderClearResource
//-------------------------------------------------------------------
void platform::shader::vertexShaderClearResource()
{
	// There's gotta be a really good reason for this to work this way, but not by passing a nullptr...
	// https://stackoverflow.com/questions/29772313/unbinding-shader-resources
	// Something tells me there's a negative to doing this...
	// https://www.gamedev.net/forums/topic/621592-unbind-render-targets-and-resource-viewes/
	ID3D11ShaderResourceView* pSrv[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	g_pRenderer->GetApi().pDeviceContext->VSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSrv );
}
//-------------------------------------------------------------------
//	pixelShaderClearResource
//-------------------------------------------------------------------
void platform::shader::pixelShaderClearResource()
{
	// There's gotta be a really good reason for this to work this way, but not by passing a nullptr...
	// https://stackoverflow.com/questions/29772313/unbinding-shader-resources
	// Something tells me there's a negative to doing this...
	// https://www.gamedev.net/forums/topic/621592-unbind-render-targets-and-resource-viewes/
	ID3D11ShaderResourceView* pSrv[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	g_pRenderer->GetApi().pDeviceContext->PSSetShaderResources( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSrv );
}

//=============================================================================
//	Helper function definition
//=============================================================================

//-------------------------------------------------------------------
//	compileShader
//-------------------------------------------------------------------
void compileShader(const wchar* file, const char* shaderTypeVersion, ID3DBlob** ppByteCodeOut)
{
	ID3DBlob* pError = nullptr;

	HRESULT hResult = D3DCompileFromFile( file, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", shaderTypeVersion, 
	#ifdef DEBUG_SHADER
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
	#else
		0,
	#endif
		0, ppByteCodeOut, &pError );

	if ( FAILED(hResult) )
	{
		if ( pError )
		{
			String msg = "Error compiling shader " + String(file) + 
				"\nMessage: " + String((const char*)pError->GetBufferPointer(), (uint)pError->GetBufferSize());
			THROW( msg.AsChar() );
		}
		else
		{
			THROW( String("File not found: " + String(file)).AsChar() );
		}
	}
}
//-------------------------------------------------------------------
//	dx11Format
//-------------------------------------------------------------------
DXGI_FORMAT dx11Format(const VertexLayoutElement& element)
{
	switch (element.type)
	{
		case eVertexLayoutType::Float:
			switch (element.typeCount)
			{
				case 1: return DXGI_FORMAT_R32_FLOAT;
				case 2: return DXGI_FORMAT_R32G32_FLOAT;
				case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
				case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			break;
	}

	THROW( "Unrecognized vertex layout type" );

	return DXGI_FORMAT_UNKNOWN;
}
//-------------------------------------------------------------------
//	dx11SemanticName
//-------------------------------------------------------------------
const char* dx11SemanticName(const VertexLayoutElement& element)
{
	switch ( element.semantic )
	{
		case eVertexLayoutSemantic::Position: return "POSITION";
		case eVertexLayoutSemantic::Color: return "COLOR";
		case eVertexLayoutSemantic::Normal: return "NORMAL";
		case eVertexLayoutSemantic::Tangent: return "TANGENT";
		case eVertexLayoutSemantic::Binormal: return "BINORMAL";
		case eVertexLayoutSemantic::TexCoord: return "TEXCOORD";
	}

	THROW( "Unrecognized vertex layout semantic" );

	return "";
}
//-------------------------------------------------------------------
//	convertToDx11Layout
//-------------------------------------------------------------------
D3D11_INPUT_ELEMENT_DESC convertToDx11Layout(const VertexLayoutElement& element)
{
	D3D11_INPUT_ELEMENT_DESC result = {};

	result.SemanticName = dx11SemanticName( element );
	result.SemanticIndex = element.semanticIndex;
	result.Format = dx11Format( element );
	result.InputSlot = 0;
	result.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	result.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	result.InstanceDataStepRate = 0;

	return result;
}
//-------------------------------------------------------------------
//	createLayoutFromVertex
//-------------------------------------------------------------------
Array<D3D11_INPUT_ELEMENT_DESC> createLayoutFromVertex(const Array<VertexLayoutElement>& elements)
{
	Array<D3D11_INPUT_ELEMENT_DESC> result;

	for ( uint i = 0; i < elements.Count(); i++ )
		result += convertToDx11Layout(elements[i]);

	result[0].AlignedByteOffset = 0;

	return result;
}


#endif
