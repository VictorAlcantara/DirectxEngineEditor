#include "SamplerState.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"

#ifdef WIN32_DX11

//-------------------------------------------------------------------
//	filterToDxFilter
//-------------------------------------------------------------------
D3D11_FILTER filterToDxFilter(eTextureFilter filter)
{
	switch (filter)
	{
		case eTextureFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case eTextureFilter::Linear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case eTextureFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
		default:
			THROW( "filterToDxFilter() Unrecognized filter" );
	}
}
//-------------------------------------------------------------------
//	addressToDxAddress
//-------------------------------------------------------------------
D3D11_TEXTURE_ADDRESS_MODE addressToDxAddress(eTextureAddress mode)
{
	switch (mode)
	{
		case eTextureAddress::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
		case eTextureAddress::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
		case eTextureAddress::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
		default:
			THROW( "addressToDxAddress() Unrecognized texture address mode" );
	}
}

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
SamplerState::SamplerState()
{
	m_initialized = false;

	for ( uint i = 0; i < engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX; i++ )
	{
		m_currentAddressU[i] = eTextureAddress::Wrap;
		m_currentAddressV[i] = eTextureAddress::Wrap;
		m_currentFilter[i] = eTextureFilter::Point;
	}
}
//-------------------------------------------------------------------
SamplerState::~SamplerState()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void SamplerState::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( uint addressU = 0; addressU < (uint)eTextureAddress::Count; addressU++ )
	{
		for ( uint addressV = 0; addressV < (uint)eTextureAddress::Count; addressV++ )
		{
			for ( uint filter = 0; filter < (uint)eTextureFilter::Count; filter++ )
			{
				platform::release( m_api[addressU][addressV][filter] );
			}

		}
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void SamplerState::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	for ( uint addressU = 0; addressU < (uint)eTextureAddress::Count; addressU++ )
	{
		for ( uint addressV = 0; addressV < (uint)eTextureAddress::Count; addressV++ )
		{
			for ( uint filter = 0; filter < (uint)eTextureFilter::Count; filter++ )
			{
				D3D11_SAMPLER_DESC samplerDesc = {};
				samplerDesc.Filter = filterToDxFilter((eTextureFilter)filter);
				samplerDesc.AddressU = addressToDxAddress((eTextureAddress)addressU);
				samplerDesc.AddressV = addressToDxAddress((eTextureAddress)addressV);
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MipLODBias = 0;
				samplerDesc.MaxAnisotropy = filter == (uint)eTextureFilter::Anisotropic ? 16 : 1;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = 0;

				HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateSamplerState( &samplerDesc, &m_api[addressU][addressV][filter].pSamplerState );

				if ( FAILED(hResult) )
				{
					THROW( "SamplerState::Initialize() Failed creating texture 2d sampler state" );
				}
			}
		}
	}

	for ( uint i = 0; i < engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX; i++ )
	{
		m_currentAddressU[i] = eTextureAddress::None;
		m_currentAddressV[i] = eTextureAddress::None;
		m_currentFilter[i] = eTextureFilter::None;
	}

	SetSamplerState( eTextureAddress::Wrap, eTextureAddress::Wrap, eTextureFilter::Linear, 0 );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	SetSamplerState
//-------------------------------------------------------------------
void SamplerState::SetSamplerState(eTextureAddress addressU, eTextureAddress addressV, eTextureFilter filter, uint slot)
{
	#ifdef DEBUG_MODE
		CHECK( slot < engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX );
	#endif

	if ( m_currentAddressU[slot] != addressU || m_currentAddressV[slot] != addressV || m_currentFilter[slot] != filter )
	{
		g_pRenderer->GetApi().pDeviceContext->PSSetSamplers( slot, 1, &m_api[(uint)addressU][(uint)addressV][(uint)filter].pSamplerState );

		m_currentAddressU[slot] = addressU;
		m_currentAddressV[slot] = addressV;
		m_currentFilter[slot] = filter;
	}
}


#endif
