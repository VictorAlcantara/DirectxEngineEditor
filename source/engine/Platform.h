#pragma once

#ifdef _DEBUG
	#define DEBUG_MODE
#else
	#undef DEBUG_MODE
#endif

#ifndef WINDOWS
	#define WINDOWS
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#define NODRAWTEXT
	#define NOBITMAP
	#define NOMCX
	#define NOSERVICE
	#define NOHELP

	#include <Windows.h>
	#include <fstream>
	#include <iostream>
	#include <string>

	#include "Type.h"

	namespace platform
	{
		struct InitializationApi
		{
			HINSTANCE hInstance;

			InitializationApi()
			{
				hInstance = 0;
			}
		};

		struct ApplicationApi
		{
			char* className;
			char* windowName;
			HINSTANCE hInstance;
			HWND hWnd;
			uint fps;
			uint width;
			uint height;

			ApplicationApi()
			{
				className = nullptr;
				windowName = nullptr;
				hInstance = 0;
				hWnd = 0;
				fps = 60;
				width = 800;
				height = 600;
			}
		};

		struct FileWriteApi
		{
			std::ofstream file;
		};

		struct FileReadApi
		{
			std::ifstream file;
			FILE* pFile;

			FileReadApi()
			{
				pFile = nullptr;
			}
		};
	};

#endif

#ifndef DIRECTX11
	#define DIRECTX11

	#include <dxgi.h>
	#include <d3d11.h>
	#include <d3dcompiler.h>

	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "d3dcompiler.lib")

	namespace platform
	{
		template <class COM_PTR>
		inline void safeComRelease(COM_PTR& pCom)
		{
			if ( pCom )
				pCom->Release();
			pCom = nullptr;
		}

		struct RendererApi
		{
			IDXGISwapChain* pSwapChain;
			ID3D11Device* pDevice;
			ID3D11DeviceContext* pDeviceContext;
			ID3D11RenderTargetView* pRenderTargetView;

			RendererApi()
			{
				pSwapChain = nullptr;
				pDevice = nullptr;
				pDeviceContext = nullptr;
				pRenderTargetView = nullptr;
			}
		};

		struct RasterizerApi
		{
			ID3D11RasterizerState* pRasterizerState;

			RasterizerApi()
			{
				pRasterizerState = nullptr;
			}
		};

		struct DepthStencilApi
		{
			ID3D11DepthStencilView* pDepthStencilView;
			ID3D11ShaderResourceView* pDepthStencilTextureView;

			DepthStencilApi()
			{
				pDepthStencilView = nullptr;
				pDepthStencilTextureView = nullptr;
			}
		};

		struct DepthStencilStateApi
		{
			ID3D11DepthStencilState* pDepthStencilState;

			DepthStencilStateApi()
			{
				pDepthStencilState = nullptr;
			}
		};

		struct ModelApi
		{
			ID3D11Buffer* pVertexBuffer;
			ID3D11Buffer* pIndexBuffer;
			D3D11_PRIMITIVE_TOPOLOGY topology;

			ModelApi()
			{
				pVertexBuffer = nullptr;
				pIndexBuffer = nullptr;
				topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			}
		};

		struct SamplerStateApi
		{
			ID3D11SamplerState* pSamplerState;

			SamplerStateApi()
			{
				pSamplerState = nullptr;
			}
		};

		struct TextureApi
		{
			ID3D11Texture2D* pTexture;
			ID3D11ShaderResourceView* pTextureView;

			TextureApi()
			{
				pTexture = nullptr;
				pTextureView = nullptr;
			}
		};

		struct RenderTargetApi
		{
			ID3D11RenderTargetView* pRenderTargetView;

			RenderTargetApi()
			{
				pRenderTargetView = nullptr;
			}
		};

		struct ShaderApi
		{
			ID3D11VertexShader* pVertexShader;
			ID3D11PixelShader* pPixelShader;
			ID3D11InputLayout* pLayout;

			ShaderApi()
			{
				pVertexShader = nullptr;
				pPixelShader = nullptr;
				pLayout = nullptr;
			}
		};

		struct ConstantBufferApi
		{
			ID3D11Buffer* pConstantBuffer;

			ConstantBufferApi()
			{
				pConstantBuffer = nullptr;
			}
		};

		struct BlendStateApi
		{
			ID3D11BlendState* pBlendState;

			BlendStateApi()
			{
				pBlendState = nullptr;
			}
		};

	};
#endif

#if defined(WINDOWS) && defined(DIRECTX11)
	#define WIN32_DX11
#endif

namespace platform
{
	void release(RendererApi&);
	void release(RasterizerApi&);
	void release(DepthStencilApi&);
	void release(DepthStencilStateApi&);
	void release(ModelApi&);
	void release(SamplerStateApi&);
	void release(TextureApi&);
	void release(RenderTargetApi&);
	void release(ShaderApi&);
	void release(ConstantBufferApi&);
	void release(BlendStateApi&);
};
