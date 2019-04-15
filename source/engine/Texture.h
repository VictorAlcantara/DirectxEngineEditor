#pragma once

#include "Platform.h"
#include "Type.h"
#include "../engine/Math.h"
#include "../engine/TgaLoader.h"
#include "../engine/String.h"

class TextureManager;

/****************************************************************************************
	ITexture
*****************************************************************************************/

class ITexture
{
	public:
		virtual ~ITexture() = default;
		virtual void Bind(uint slot) = 0;

		virtual void Shutdown() = 0;

		virtual uint GetWidth()const = 0;
		virtual uint GetHeight()const = 0;
		virtual math::Vec2 GetDimension()const = 0;

		virtual eTextureType GetType()const = 0;

		virtual const HashString& GetName()const = 0;

		virtual void Copy(ITexture*) = 0;

		virtual platform::TextureApi& GetApi() = 0;
};

/****************************************************************************************
	TextureBase
*****************************************************************************************/

class TextureBase : public ITexture
{
	friend class TextureManager;

	public:
		virtual ~TextureBase();
		
		virtual void Bind(uint slot)override final;
		virtual void Shutdown()override final;

		virtual void Copy(ITexture*)override final;

		uint GetWidth()const override final { return m_width; }
		uint GetHeight()const override final { return m_height; }
		math::Vec2 GetDimension()const override final { return math::Vec2((float)m_width, (float)m_height); }

		platform::TextureApi& GetApi()override final;

		const HashString& GetName()const override final;

	protected:
		TextureBase();
		void SetName(const HashString& name);

		bool m_initialized;
		platform::TextureApi m_api;
		HashString m_name;
		uint m_width;
		uint m_height;
};

/****************************************************************************************
	Texture2d
*****************************************************************************************/

class Texture2d final : public TextureBase
{
	friend class TextureManager;

	public:
		eTextureType GetType()const override final { return eTextureType::Texture2d; }

	private:
		void InitializeEmpty(uint width, uint height, eColorFormat);
		void Initialize(const uchar* pData, uint width, uint height, uint bitsPerPixel);
		void Initialize(const TgaInfo&);
};

/****************************************************************************************
	TextureRenderTarget
*****************************************************************************************/

class TextureRenderTarget final : public TextureBase
{
	friend class TextureManager;

	public:
		eTextureType GetType()const override final { return eTextureType::RenderTarget; }

	private:
		void Initialize(uint width, uint height, eColorFormat);

};

/****************************************************************************************
	TextureDepthStencil
*****************************************************************************************/

class TextureDepthStencil final : public TextureBase
{
	friend class TextureManager;

	public:
		eTextureType GetType()const override final { return eTextureType::DepthStencil; }

	private:
		void Initialize(uint width, uint height);

};


