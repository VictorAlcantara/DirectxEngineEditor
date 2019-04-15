#pragma once

#include "GuiUtil.h"
#include "../engine/Math.h"
#include "../engine/List.h"
#include "../engine/String.h"


class GuiAnimation
{
	private:
		struct GuiAnimationSequence
		{
			GuiAnimationData data;
			ushort uiTimeScale;
			float fTimeScale;
			float duration;

			GuiAnimationSequence() : uiTimeScale(0), fTimeScale(0.0f), duration(0.0f) {}
		};

	public:
		void Add(const GuiAnimationData&, ushort timeScaleZeroToOneHundred);
		void Remove(ushort timeScaleZeroToOneHundred);
		GuiAnimationData Get(float progress, eGuiPageState)const;

	private:
		GuiAnimationData Interpolate(const GuiAnimationData&, float progress)const;

		List<GuiAnimationSequence> m_sequences;
};
