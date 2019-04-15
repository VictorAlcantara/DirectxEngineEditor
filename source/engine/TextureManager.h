#pragma once

#include "ResourceModule.h"
#include "Class.h"
#include "Handle.h"
#include "Texture.h"
#include "Table.h"
#include "RenderUtil.h"
#include "EventListener.h"

class TextureManager : public NonCopyable, public EventListener
{
	RESOURCE_MODULE;

	public:
		~TextureManager();

		void LoadDirectory(const String&);
		Handle<ITexture> CreateFromFile(const String& path, bool logDuplicate = true);
		Handle<ITexture> CreateEmpty(const String& name, uint width, uint height, eColorFormat, bool logDuplicate = true);
		Handle<ITexture> CreateFromStream(const String& name, const uchar* pData, uint width, uint height, uint bitsPerPixel, bool logDuplicate = true);
		Handle<ITexture> CreateRenderTargetTexture(const String& name, uint width, uint height, eColorFormat);
		Handle<ITexture> CreateDepthStencilTexture(const String& name, uint width, uint height);

		void Remove(Handle<ITexture>);
		void Remove(const HashString& name);

		Handle<ITexture> Get(const HashString&);
		Handle<ITexture> Get(uint);

		TableIteratorConst<Handle<ITexture>> GetAll()const;

		void HandleEvent(Event*)override final;

	private:
		TextureManager();

		void Initialize();
		void Shutdown();

		bool m_initialized;
		Table<Handle<ITexture>, uint> m_table;
		HandleManager<ITexture> m_handles;
		String m_directoryPath;
};
