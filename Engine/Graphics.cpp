/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "D3D10Backend.h"
#include "D3D11Backend.h"
#include "Graphics.h"
#include <assert.h>
#include <string>
#include "simd.h"

// allocate memory for sysbuffer (16-byte aligned for faster access)
Graphics::Graphics( HWNDKey& key )
	:
	pSysBuffer( reinterpret_cast< Color* >(
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight, 16u ) ) ),
	screen( Matrix<3, 2, float>::Scaling( { ( float( ScreenWidth ) / float( ScreenHeight ) ), -1.f } ) *
		Matrix<3, 2, float>::Translation( Vec2f{ float( ScreenWidth ), float( ScreenHeight ) } *.5f ) )
{
	try
	{
		backend = std::make_unique<D3D11Backend>( key.hWnd );
	}
	catch( const D3DBackend::Exception& e )
	{
		if(e.GetErrorName() == L"DXGI_ERROR_UNSUPPORTED")
		{
			backend = std::make_unique<D3D10Backend>( key.hWnd );
		}
		else
		{
			throw;
		}
	}

	quad[ 0 ] = Vertex{ { -.5f,  .5f }, { 0.f, 0.f } };		// Top left
	quad[ 1 ] = Vertex{ {  .5f,  .5f }, { 1.f, 0.f } };		// Top right
	quad[ 2 ] = Vertex{ { -.5f, -.5f }, { 0.f, 1.f } };		// Bottom left
	quad[ 3 ] = Vertex{ {  .5f, -.5f }, { 1.f, 1.f } };		// Bottom right
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
}

void Graphics::EndFrame()
{
	backend->Present( pSysBuffer );
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x, int y, int r, int g, int b )
{
	PutPixel( x, y, { unsigned char( r ), unsigned char( g ), unsigned char( b ) } );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

Color Graphics::GetPixel( int x, int y )const
{
	return pSysBuffer[ Graphics::ScreenWidth * y + x ];
}

void Graphics::PutPixelAlpha( int x, int y, Color c )
{
	const auto ca = c.GetA();
	const auto da = 255 - ca;
	const auto dst = GetPixel( x, y );
	if( ca > 0 && ca < 255 )
		int a = 0;
	const Color result = {
		unsigned char( ( ( c.GetR() * ca ) + ( dst.GetR() * da ) ) >> 8 ),
		unsigned char( ( ( c.GetG() * ca ) + ( dst.GetG() * da ) ) >> 8 ),
		unsigned char( ( ( c.GetB() * ca ) + ( dst.GetB() * da ) ) >> 8 )
	};

	PutPixel( x, y, result );
}

void Graphics::DrawSprite( const Matrix<3, 2, float>& transform, Sprite sprite )
{
	DrawTriangle( transform, sprite );
}
	 
void Graphics::DrawTriangle( const Matrix<3, 2, float>& transform, Sprite sprite )
{
	auto Rasterize = [ this, sprite ]( const int X, const int Y, const Vec2f& P, const Triangle<x86>& tri )
	{
		const auto coord = tri.GetBaryCoords( P );
		if( coord.IsBarycentric() )
		{
			const auto tc = coord.Interpolate( tri[ 0 ].tex, tri[ 1 ].tex, tri[ 2 ].tex );

			const int tx = int( tc.x * ( sprite.width() - 1.f ) );
			const int ty = int( tc.y * ( sprite.height() - 1.f ) );

			PutPixelAlpha( X, Y, sprite.pixel( tx, ty ) );
		}
	};
	
	auto sseRasterize = 
		[ this, sprite ]( const int X, const int Y, const Vec2SSE P, const Triangle<SSE>& tri )
	{
		// Get texture dimensions
		float128 texWidth( sprite.width() - 1.f );
		float128 texHeight( sprite.height() - 1.f );

		const auto coords = tri.GetBaryCoords( P );

		const auto v0 = tri.GetVertexTexcoord( 0 );
		const auto v1 = tri.GetVertexTexcoord( 1 );
		const auto v2 = tri.GetVertexTexcoord( 2 );

		// Interpolate texture coordinates
		Vec2SSE tc = coords.Interpolate(
			tri.GetVertexTexcoord( 0 ),
			tri.GetVertexTexcoord( 1 ),
			tri.GetVertexTexcoord( 2 )
		);

		tc.x *= texWidth;
		tc.y *= texHeight;

		// Convert to integers
		int128 itx = int128( tc.x );
		int128 ity = int128( tc.y );

		// Store results
		alignas( 16 ) int tcx[ 4 ], tcy[ 4 ];
		itx.Store( tcx );
		ity.Store( tcy );

		// Loop through results, skipping any that were masked out
		for( int i = 0; i < 4; ++i )
		{
			if( coords.IsBarycentric( i ) )
			{
				const Color c = sprite.pixel( tcx[ i ], tcy[ i ] );
				PutPixelAlpha( X + i, Y, c );
			}
		}
	};

	// Transform the vertices
	std::array<Vertex, 4> tverts;
	std::transform( quad.begin(), quad.end(), tverts.begin(), [ &transform ]( const Vertex& v )
		{
			Vertex out;
			out.pos = transform * v.pos;
			out.tex = v.tex;

			return out;
		} );

	// Transform to screen space
	std::transform( tverts.begin(), tverts.end(), tverts.begin(), [ this ]( const Vertex& v )
		{
			Vertex out;
			out.pos = screen * v.pos;
			out.tex = v.tex;

			return out;
		} );
	
	// Pack vertices into a Triangle object where the area will be calculated upon construction
	std::array<Triangle<x86>, 2> tris = {
		Triangle<x86>( tverts[ 0 ], tverts[ 1 ], tverts[ 2 ] ),
		Triangle<x86>( tverts[ 3 ], tverts[ 2 ], tverts[ 1 ] )
	};

	// Loop through bounding box
	const auto box = BoundingBox( tverts );
	const auto rect = Clipper()( box, GetScreenRect() );

	auto x86Rasterize = [ &Rasterize, tris ]( int ix, int iy )
	{
		const Vec2f p = Vec2f( float( ix ), float( iy ) );
		Rasterize( ix, iy, p, tris[ 0 ] );
		Rasterize( ix, iy, p, tris[ 1 ] );
	};
	auto m128Rasterize = [ this, &sseRasterize, tris ]( int ix, int iy )
	{
		const Vec2f p = Vec2f( float( ix ), float( iy ) );
		Vec2SSE mp( float128( p.x + 3.f, p.x + 2.f, p.x + 1.f, p.x ), float128( p.y ) );

		sseRasterize( ix, iy, mp, { tris[ 0 ] } );
		sseRasterize( ix, iy, mp, { tris[ 1 ] } );
	};
	//for_each( rect, x86Rasterize );
	simd_for_each<4>( rect.left, rect.top, rect.right, rect.bottom, m128Rasterize, x86Rasterize );
}
Rect<int> Graphics::GetScreenRect()const
{
	return { 0, 0, ScreenWidth, ScreenHeight };
}