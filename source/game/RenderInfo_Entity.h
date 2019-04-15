#pragma once

#include "../engine/RenderInfo.h"

struct RenderInfo_Entity : public RenderInfo_EntitySceneBase
{
	RENDER_INFO_DECL( RenderInfo_Entity );

	RenderInfo_Entity();

	Handle<ITexture> hDiffuseMap;
	Handle<ITexture> hSpecularMap;
	Handle<ITexture> hNormalMap;
	TextureSampler sampler;
};
