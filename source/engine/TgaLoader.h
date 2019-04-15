#pragma once

#include "Type.h"
#include "../engine/String.h"
#include "../engine/Pointer.h"
#include "RenderUtil.h"

struct TgaInfo
{
	uchar* pData;
	uint dataSize;
	eColorComponent colorComponent;
	uint width;
	uint height;
	uint bitsPerPixel;

	TgaInfo() : pData(nullptr), dataSize(0), colorComponent(eColorComponent::None), width(0), height(0), bitsPerPixel(0) {}
	~TgaInfo()
	{
		safeArrayRelease( pData );
	}
};

class TgaLoader
{
	public:
		TgaLoader();

		bool Load(const String&);
		bool IsLoaded()const { return m_loaded; }
		const TgaInfo& GetInfo()const;
		
	private:
		bool m_loaded;
		TgaInfo m_data;
};



