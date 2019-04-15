#include "GuiAnimation.h"
#include "../engine/Debug.h"

//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void GuiAnimation::Add(const GuiAnimationData& data, ushort uiTimeScale)
{
	CHECK( uiTimeScale >= 0 && uiTimeScale <= 100 );

	if ( uiTimeScale > 100 )
		uiTimeScale = 100;

	auto pNode = m_sequences.GetFirst();

	for ( ; pNode; pNode = pNode->GetNext() )
	{
		CHECK( uiTimeScale != pNode->GetValue().uiTimeScale );

		if ( uiTimeScale <= pNode->GetValue().uiTimeScale )
		{
			GuiAnimationSequence sequence;
			sequence.data = data;
			sequence.uiTimeScale = uiTimeScale;
			sequence.fTimeScale = sequence.uiTimeScale * 0.01f;

			pNode->AddPrev( sequence );
			break;
		}
	}

	if ( !pNode )
	{
		GuiAnimationSequence sequence;
		sequence.data = data;
		sequence.uiTimeScale = uiTimeScale;
		sequence.fTimeScale = sequence.uiTimeScale * 0.01f;

		m_sequences.AddLast( sequence );
	}

	// Calculate duration
	for ( pNode = m_sequences.GetLast(); pNode; pNode = pNode->GetPrev() )
	{
		float fTimeScalePrev = pNode->GetPrev() ? pNode->GetPrev()->GetValue().fTimeScale : 0.0f;

		pNode->GetValue().duration = pNode->GetValue().fTimeScale - fTimeScalePrev;
	}
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void GuiAnimation::Remove(ushort uiTimeScale)
{
	for ( auto pNode = m_sequences.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue().uiTimeScale == uiTimeScale )
		{
			pNode->Remove();
			break;
		}
	}
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
GuiAnimationData GuiAnimation::Get(float progress, eGuiPageState state)const
{
	GuiAnimationData result;

	if ( state == eGuiPageState::Visible )
	{
		result = m_sequences.GetLast()->GetValue().data;
	}
	else if ( state == eGuiPageState::Invisible )
	{
		result = m_sequences.GetFirst()->GetValue().data;
	}
	else if ( state == eGuiPageState::Showing )
	{
		result = m_sequences.GetFirst()->GetValue().data;
	
		result = Interpolate( result, progress );
	}
	else if ( state == eGuiPageState::Hiding )
	{
		result = m_sequences.GetLast()->GetValue().data;
	
		result = Interpolate( result, 1.0f - progress );
	}
	
	return result;
}
//-------------------------------------------------------------------
//	Interpolate
//-------------------------------------------------------------------
GuiAnimationData GuiAnimation::Interpolate(const GuiAnimationData& animInput, float progress)const
{
	float timeScaleLast = 0.0f;
	GuiAnimationData result;
	GuiAnimationData animBase = animInput;

	CHECK( m_sequences.Count() == 0 || m_sequences.GetFirst()->GetValue().uiTimeScale == 0 && m_sequences.GetLast()->GetValue().uiTimeScale == 100 );
	CHECK( progress >= 0.0f && progress <= 1.0f )

	for ( auto it = m_sequences.GetIterator(); it; it++ )
	{
		GuiAnimationSequence currSequence =	*it;
		float timeScale = (float)currSequence.fTimeScale;

		if ( progress > timeScale || currSequence.uiTimeScale == 0 )
		{
			timeScaleLast = timeScale;
			animBase.scale = currSequence.data.scale;
			animBase.alpha = currSequence.data.alpha;
			animBase.saturate = currSequence.data.saturate;

			continue;
		}

		float progressSequence = (progress - timeScaleLast) / currSequence.duration;

		CHECK( progressSequence >= 0.0f && progressSequence <= 1.0f )

		result.scale = math::smoothHermite( animBase.scale, currSequence.data.scale, progressSequence );
		result.alpha = math::smoothHermite( animBase.alpha, currSequence.data.alpha, progressSequence );
		result.saturate = math::smoothHermite( animBase.saturate, currSequence.data.saturate, progressSequence );

		break;
	}

	return result;
}
