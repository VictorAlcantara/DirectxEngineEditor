#include "RenderInfoEditor.h"

/****************************************************************************************
	RenderInfo
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
RenderInfo_EditorEntity::RenderInfo_EditorEntity() : RenderInfo_EntitySceneBase(0, 0, 0, 0),
	renderBoundingSphere(false),
	boundingSphereScale(1.0f),
	renderDirection(false),
	renderAabb(false)
{
	UseTexture( 3 );
	AddTexture( &hDiffuseTexture, &sampler, (renderFlag)eRenderFlagTexture::Ambient | (renderFlag)eRenderFlagTexture::Diffuse );
	AddTexture( &hSpecularTexture, &sampler, (renderFlag)eRenderFlagTexture::Specular );
	AddTexture( &hNormalMap, &sampler, (renderFlag)eRenderFlagTexture::NormalMap );
}
