#pragma once

#include "D3DBackend.h"
#include <d3d10_1.h>

class D3D10Backend : public D3DBackend
{
public:
	D3D10Backend( HWND Handle );
	~D3D10Backend();

	void Present( const Color* pSysBuffer ) override;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D10Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D10RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D10Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D10ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D10PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D10VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D10Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D10InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D10SamplerState>			pSamplerState;
	D3D10_MAPPED_TEXTURE2D								mappedSysBufferTexture;
};

