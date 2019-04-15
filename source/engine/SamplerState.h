#pragma once

#include "Platform.h"
#include "RenderUtil.h"
#include "../engine/EngineGlobal.h"

class Renderer;

// TODO: decouple api call from class definition
class SamplerState
{
	friend class Renderer;

	public:
		~SamplerState();

	private:
		SamplerState();

		void Initialize();
		void Shutdown();

		void SetSamplerState(eTextureAddress addressU, eTextureAddress addressV, eTextureFilter, uint slot);

		bool m_initialized;
		platform::SamplerStateApi m_api[(uint)eTextureAddress::Count][(uint)eTextureAddress::Count][(uint)eTextureFilter::Count];
		eTextureAddress m_currentAddressU[engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX];
		eTextureAddress m_currentAddressV[engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX];
		eTextureFilter m_currentFilter[engineDefault::samplerState::SAMPLER_STATE_SLOT_MAX];
};
