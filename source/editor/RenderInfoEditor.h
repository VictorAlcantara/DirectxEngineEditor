#pragma once

#include "../engine/RenderInfo.h"
#include "../engine/Model.h"

class ITexture;

struct RenderInfo_EditorEntity : public RenderInfo_EntitySceneBase
{
	RENDER_INFO_DECL( RenderInfo_EditorEntity );

	RenderInfo_EditorEntity();

	Handle<ITexture> hDiffuseTexture;
	Handle<ITexture> hSpecularTexture;
	Handle<ITexture> hNormalMap;
	TextureSampler sampler;

	math::Vec4 highlightColor4;
	bool renderBoundingSphere;
	math::Vec3 boundingSphereScale;

	bool renderAabb;

	bool renderDirection;
	math::Vec3 direction;
};

struct RenderInfo_EditorTool : public RenderInfo
{
	RENDER_INFO_DECL( RenderInfo_EditorTool );

	Handle<Model> hModel;

	struct {
		math::Vec4 selectedColor4;
		math::Vec4 unselectedColor4;
	} occlusionPass;

	struct {
		math::Vec4 selectedAmbient4;
		math::Vec4 selectedDiffuse4;
		math::Vec4 unselectedAmbient4;
		math::Vec4 unselectedDiffuse4;
	} normalPass;
};