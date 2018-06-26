#include "D3D10Backend.h"
#include "Graphics.h"
#include <assert.h>
#include <d3dcompiler.h>

#pragma comment( lib,"d3d10.lib" )
#pragma comment( lib,"d3dcompiler.lib")

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.

#ifndef CHILI_GFX_EXCEPTION
#define CHILI_GFX_EXCEPTION( hr,note ) D3DBackend::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )
#endif

using Microsoft::WRL::ComPtr;

D3D10Backend::D3D10Backend( HWND Handle )
{
	assert( Handle != nullptr );	
	constexpr auto tex_format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = tex_format;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = Handle;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D10CreateDeviceAndSwapChain(
		nullptr,
		D3D10_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		D3D10_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D10Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D10Texture2D ),
		( LPVOID* )&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pDevice->OMSetRenderTargets( 1, pRenderTargetView.GetAddressOf(), nullptr );


	// set viewport dimensions
	D3D10_VIEWPORT vp;
	vp.Width  = UINT( Graphics::ScreenWidth );
	vp.Height = UINT( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDevice->RSSetViewports( 1, &vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D10_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = tex_format;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D10_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc, nullptr, &pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating sysbuffer texture" );
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc, &pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating view on sysBuffer texture" );
	}

	// Compile the Pixel Shader
	ComPtr<ID3DBlob> psShader, pError;
	if( FAILED( hr = D3DCompileFromFile(
		L"FramebufferPS.hlsl",
		nullptr,
		nullptr,
		"FramebufferPS10",
		"ps_4_0",
		0,
		0,
		&psShader,
		&pError ) ) )
	{
		const std::string err = reinterpret_cast< char* >( pError->GetBufferPointer() );

		throw CHILI_GFX_EXCEPTION( hr, std::wstring( err.begin(), err.end() ) );
	}

	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	if( FAILED( hr = pDevice->CreatePixelShader(
		psShader->GetBufferPointer(),
		psShader->GetBufferSize(),
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating pixel shader" );
	}


	ComPtr<ID3DBlob> vsShader;
	if( FAILED( hr = D3DCompileFromFile(
		L"FramebufferVS.hlsl",
		nullptr,
		nullptr,
		"FramebufferVS",
		"vs_4_0",
		0,
		0,
		&vsShader,
		&pError ) ) )
	{
		const std::string err = reinterpret_cast< char* >( pError->GetBufferPointer() );

		throw CHILI_GFX_EXCEPTION( hr, std::wstring( err.begin(), err.end() ) );
	}

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	if( FAILED( hr = pDevice->CreateVertexShader(
		vsShader->GetBufferPointer(),
		vsShader->GetBufferSize(),
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating vertex shader" );
	}

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f, 1.0f, 0.5f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.5f, 1.0f, 0.0f },
	{ 1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
	{ -1.0f, 1.0f, 0.5f, 0.0f, 0.0f },
	{ 1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
	{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
	};
	D3D10_BUFFER_DESC bd = {};
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D10_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd, &initData, &pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating vertex buffer" );
	}


	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D10_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied, 2,
		vsShader->GetBufferPointer(),
		vsShader->GetBufferSize(),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D10_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D10_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc, &pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Creating sampler state" );
	}
}

D3D10Backend::~D3D10Backend()
{
	if( pDevice ) pDevice->ClearState();
}

void D3D10Backend::Present( const Color * pSysBuffer )
{
	HRESULT hr;
	
	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pSysBufferTexture->Map( 
		0, D3D10_MAP_WRITE_DISCARD, 0, &mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr, L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>( mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ], &pSysBuffer[ y * srcPitch ], rowBytes );
	}
	// release the adapter memory
	pSysBufferTexture->Unmap( 0 );

	// render offscreen scene texture to back buffer
	pDevice->IASetInputLayout( pInputLayout.Get() );
	pDevice->VSSetShader( pVertexShader.Get() );
	pDevice->PSSetShader( pPixelShader.Get() );
	pDevice->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pDevice->IASetVertexBuffers( 0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset );
	pDevice->PSSetShaderResources( 0u, 1u, pSysBufferTextureView.GetAddressOf() );
	pDevice->PSSetSamplers( 0u, 1u, pSamplerState.GetAddressOf() );
	pDevice->Draw( 6u, 0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u, 0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(), L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr, L"Presenting back buffer" );
		}
	}
}
