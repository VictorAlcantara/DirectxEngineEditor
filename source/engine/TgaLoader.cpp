#include "../engine/TgaLoader.h"
#include "../engine/Exception.h"
#include "Platform.h"
#include "../engine/File.h"
#include "../engine/Math.h"

// https://en.wikipedia.org/wiki/Truevision_TGA#Technical_details
struct TgaData
{
	uchar id;
	uchar colorMapType;
	uchar imageType;
	uchar colorMapSpec[5];
	struct
	{
		ushort xOrigin;
		ushort yOrigin;
		ushort width;
		ushort height;
		uchar bitsPerPixel;
		uchar descriptor;
	} imageSpec;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
TgaLoader::TgaLoader()
{
	m_loaded = false;
}
//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
bool TgaLoader::Load(const String& path)
{
	auto setColorFromTga = [](uchar* pDest, const uchar* pSource, uint bytesPerPixel)
	{
		// Convert from BGR to RGB
		pDest[0] = pSource[2];
		pDest[1] = pSource[1];
		pDest[2] = pSource[0];
		if ( bytesPerPixel == 4 )
			pDest[3] = pSource[3];
	};

	FileReader file;
	TgaData tgaData = {};
	
	if ( !file.Load( path, eFileMode::Binary ) )
	{
		ASSERT( "Failed loading texture" );
		m_loaded = false;
	}
	else
	{
		uint tgaDataSize = sizeof(TgaData);
		const uchar* pFileData = (const uchar*)file.GetLine(0).GetData();
		memoryCopy( pFileData, (uchar*)&tgaData, tgaDataSize );

		const uchar* pFileColorData = (const uchar*)(&pFileData[tgaDataSize]);
		uint bytesPerPixel = tgaData.imageSpec.bitsPerPixel / 8;
		uchar* pColorData = nullptr;


		if ( bytesPerPixel == 3 || bytesPerPixel == 4 )
		{
			if ( tgaData.imageType == 2 )
			{
				uint pixelCount = tgaData.imageSpec.width * tgaData.imageSpec.height;
				pColorData = new uchar[pixelCount * bytesPerPixel];

				for ( uint i = 0; i < pixelCount; i++ )
					setColorFromTga( &pColorData[i * bytesPerPixel], &pFileColorData[i * bytesPerPixel], bytesPerPixel );
			}
			else if ( tgaData.imageType == 10 ) // RLE compression http://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf
			{
				uint colorByteCount = tgaData.imageSpec.width * tgaData.imageSpec.height * bytesPerPixel;
				pColorData = new uchar[colorByteCount];
				uint encodedIndex = 0;

				for ( uint colorByte = 0; colorByte < colorByteCount; )
				{
					bool isEncoded = pFileColorData[encodedIndex] & 0x80;
					uint packetLength = (pFileColorData[encodedIndex] & 0x7F) + 1;
				
					encodedIndex++;

					if ( isEncoded )
					{
						for ( uint i = 0; i < packetLength; i++ )
						{
							setColorFromTga( &pColorData[colorByte], &pFileColorData[encodedIndex], bytesPerPixel );
							colorByte += bytesPerPixel;
						}

						encodedIndex += bytesPerPixel;
					}
					else
					{
						for ( uint i = 0; i < packetLength; i++ )
						{
							setColorFromTga( &pColorData[colorByte], &pFileColorData[encodedIndex], bytesPerPixel );
							colorByte += bytesPerPixel;
							encodedIndex += bytesPerPixel;
						}
					}
				}
			}
			else if ( tgaData.imageType != 2 )
			{
				ASSERT( "Invalid targa image format" );
			}
		}
		else
		{
			ASSERT( "Invalid pixel depth" );
		}

		if ( pColorData )
		{
			m_data.width = tgaData.imageSpec.width;
			m_data.height = tgaData.imageSpec.height;
			m_data.bitsPerPixel = tgaData.imageSpec.bitsPerPixel;
			m_data.dataSize = m_data.width * m_data.height * (m_data.bitsPerPixel / 8);

			// if xOrigin and yOrigin are 0, this means that its coordinates in texture space are (0, 1), thus must be inverted
			if ( tgaData.imageSpec.xOrigin == 0 && tgaData.imageSpec.yOrigin == 0 )
			{
				m_data.pData = new uchar[m_data.dataSize];

				for ( uint row = 0; row < m_data.height; row++ )
				{
					for ( uint col = 0; col < m_data.width; col++ )
					{
						uint index = row * m_data.width + col;
						uint indexInverted = (m_data.height - 1 - row) * m_data.width + col;
						for ( uint pixel = 0; pixel < bytesPerPixel; pixel++ )
						{
							m_data.pData[index * bytesPerPixel + pixel] = pColorData[indexInverted * bytesPerPixel + pixel];
						}
					}
				}

				safeArrayRelease( pColorData );
			}
			else
				m_data.pData = pColorData;

			switch( tgaData.imageSpec.bitsPerPixel )
			{
				case 24: m_data.colorComponent = eColorComponent::Rgb; break;
				case 32: m_data.colorComponent = eColorComponent::Rgba; break;
			}

			m_loaded = true;
		}
	}

	return m_loaded;
}
//-------------------------------------------------------------------
//	GetInfo
//-------------------------------------------------------------------
const TgaInfo& TgaLoader::GetInfo()const
{
	CHECK( IsLoaded() );
	return m_data;
}

