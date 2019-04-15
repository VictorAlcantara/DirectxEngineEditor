#include "RenderInfo_Entity.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
RenderInfo_Entity::RenderInfo_Entity() : RenderInfo_EntitySceneBase(0, 0, 0, 0)
{
	UseTexture( 3 );
	AddTexture( &hDiffuseMap, &sampler, (renderFlag)eRenderFlagTexture::Ambient | (renderFlag)eRenderFlagTexture::Diffuse );
	AddTexture( &hSpecularMap, &sampler, (renderFlag)eRenderFlagTexture::Specular );
	AddTexture( &hNormalMap, &sampler, (renderFlag)eRenderFlagTexture::NormalMap );
}
