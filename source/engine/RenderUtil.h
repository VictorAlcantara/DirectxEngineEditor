#pragma once

#include "Type.h"

enum class eShaderType
{
	PixelShader,
	VertexShader,
};

enum class eDepthOperation
{
	Disabled,
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,

	Count // MUST ALWAYS BE LAST! Used for array counting
};

enum class eCullMode
{
	None = 0,
	Front,
	Back,

	Count // MUST ALWAYS BE LAST! Used for array counting
};

enum class eFillMode
{
	Solid = 0,
	Wireframe,

	Count // MUST ALWAYS BE LAST! Used for array counting
};

enum class eBlendMode
{
	Disabled = 0,
	Alpha,
	Additive,

	Count // MUST ALWAYS BE LAST! Used for array counting
};

enum class eTextureAddress
{
	Wrap = 0,
	Mirror,
	Clamp,

	Count, // Used for indexing
	None
};

enum class eTextureFilter
{
	Point = 0,
	Linear,
	Anisotropic,

	Count, // Used for indexing
	None
};

enum class eColorComponent
{
	None,
	Rgb,
	Rgba,
};

enum class eColorFormat
{
	None,
	Rgb_8_Uint,
	Rgba_8_Uint,
	Rg_32_Float,
	Rgb_32_Float,
	Rgba_32_Float,
};

enum class eTextureType
{
	Texture2d,
	RenderTarget,
	DepthStencil,
};

enum class eTextureGroup
{
	ColorMap,
};

enum class eTopology
{
	None,
	Point,
	Line,
	LineStrip,
	Triangle,
	TriangleStrip,
};

struct TextureSampler
{
	eTextureAddress u;
	eTextureAddress v;
	eTextureFilter filter;

	TextureSampler() : u(eTextureAddress::Wrap), v(eTextureAddress::Wrap), filter(eTextureFilter::Point) {}
};
