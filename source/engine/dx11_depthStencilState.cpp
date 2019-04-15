#include "api_DepthStencilState.h"

#ifdef WIN32_DX11

#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	Helper function
*****************************************************************************************/

BOOL depthOperationToDx11DepthEnabled(eDepthOperation);
D3D11_COMPARISON_FUNC depthOperationToDx11DepthFunction(eDepthOperation);

//-------------------------------------------------------------------
//	depthOperationToDx11DepthStencil
//-------------------------------------------------------------------
void depthOperationToDx11DepthStencil(eDepthOperation depthOperation, bool allowWriteToDepthStencilBuffer, platform::DepthStencilStateApi& depthStencilApi, platform::RendererApi& rendererApi)
{
	HRESULT hResult;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = depthOperationToDx11DepthEnabled(depthOperation);
	depthStencilDesc.DepthWriteMask =  allowWriteToDepthStencilBuffer ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = depthOperationToDx11DepthFunction(depthOperation);
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hResult = rendererApi.pDevice->CreateDepthStencilState( &depthStencilDesc, &depthStencilApi.pDepthStencilState );
	if ( FAILED(hResult) )
	{
		THROW( "Failed creating depth stencil state" );
	}
}
//-------------------------------------------------------------------
//	depthOperationToDx11DepthEnabled
//-------------------------------------------------------------------
BOOL depthOperationToDx11DepthEnabled(eDepthOperation depthOp)
{
	return depthOp == eDepthOperation::Disabled ? FALSE : TRUE;
}
//-------------------------------------------------------------------
//	depthOperationToDx11DepthFunction
//-------------------------------------------------------------------
D3D11_COMPARISON_FUNC depthOperationToDx11DepthFunction(eDepthOperation depthOp)
{
	switch ( depthOp )
	{
		case eDepthOperation::Disabled: return D3D11_COMPARISON_NEVER;
		case eDepthOperation::Never: return D3D11_COMPARISON_NEVER;
		case eDepthOperation::Less: return D3D11_COMPARISON_LESS;
		case eDepthOperation::Equal: return D3D11_COMPARISON_EQUAL;
		case eDepthOperation::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
		case eDepthOperation::Greater: return D3D11_COMPARISON_GREATER;
		case eDepthOperation::NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
		case eDepthOperation::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
		case eDepthOperation::Always: return D3D11_COMPARISON_ALWAYS;

		default:
			THROW( "depthOperationToDx11DepthFunction() invalid depth operation" );
	}
}
//-------------------------------------------------------------------
//	getIndex
uint getIndex(uint allowWrite, uint operation)
{
	CHECK( allowWrite < 2 && operation < (uint)eDepthOperation::Count );
	
	return allowWrite * (uint)eDepthOperation::Count + (uint)operation;
}

/****************************************************************************************
	namespace definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	createDepthStencilState
//-------------------------------------------------------------------
uint platform::depthStencilState::createDepthStencilState(DepthStencilStateApi** states)
{
	uint result = 2 * (uint)eDepthOperation::Count;
	*states = new DepthStencilStateApi[result];

	for ( uint allowWrite = 0; allowWrite < 2; allowWrite++ )
	{
		for ( uint i = 0; i < (uint)eDepthOperation::Count; i++ )
			depthOperationToDx11DepthStencil( (eDepthOperation)i, allowWrite, (*states)[getIndex(allowWrite, i)], g_pRenderer->GetApi() );
	}

	return result;
}
//-------------------------------------------------------------------
//	setOperation
//-------------------------------------------------------------------
void platform::depthStencilState::setOperation(eDepthOperation op, bool allowWrite, DepthStencilStateApi* states, uint count)
{
	CHECK( count == 2 * (uint)eDepthOperation::Count );

	g_pRenderer->GetApi().pDeviceContext->OMSetDepthStencilState( states[getIndex(allowWrite, (uint)op)].pDepthStencilState, 1 );
}

#endif
