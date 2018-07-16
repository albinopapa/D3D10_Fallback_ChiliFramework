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
#include "template_math_ops.h"

// allocate memory for sysbuffer (16-byte aligned for faster access)
Graphics::Graphics( HWNDKey& key )
	:
	pSysBuffer( reinterpret_cast< Color* >(
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight, 16u ) ) ),
	screen( Matrix<3, 2, float>::Scaling( { 1.f, -1.f } ) *
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

	const Color result = {
		unsigned char( ( ( c.GetR() * ca ) + ( dst.GetR() * da ) ) >> 8 ),
		unsigned char( ( ( c.GetG() * ca ) + ( dst.GetG() * da ) ) >> 8 ),
		unsigned char( ( ( c.GetB() * ca ) + ( dst.GetB() * da ) ) >> 8 )
	};

	PutPixel( x, y, result );
}

void Graphics::DrawLine( const Vec2f & p0, const Vec2f & p1, const float thickness, const Sprite& sprite )
{
	const Vec2f a = p1 - p0;
	const Vec2f aHat = Normalize( a );
	const float aLen = DotProduct( a, aHat );
	const Vec2f offset = p0 + ( a * .5f );
	const auto rotation = [ aHat ]()
	{
		Matrix3x2f rotation;
		rotation.Row( 0 ) = { aHat.x, aHat.y };
		rotation.Row( 1 ) = { -aHat.y, aHat.x };
		rotation.Row( 2 ) = { 0.f, 0.f };
		return rotation;
	}( );

	const Matrix3x2f mat = Matrix3x2f::Scaling( { aLen, thickness } ) * rotation * Matrix3x2f::Translation( offset );

	DrawTriangle( mat, sprite, [ this ]( int x, int y, Color c )
		{
			PutPixel( x, y, c );
		} );
}

Rect<int> Graphics::GetScreenRect()const
{
	return { 0, 0, ScreenWidth, ScreenHeight };
}

std::array<Vertex, 4> Graphics::TransformVertices( const Matrix<3, 2, float>& transform )const
{
	// Transform the vertices
	const auto screenTransform = transform * screen;
	std::array<Vertex, 4> tverts;
	std::transform( quad.begin(), quad.end(), tverts.begin(), [ &screenTransform ]( const Vertex& v )
		{
			Vertex out;
			out.pos = screenTransform * v.pos;
			out.tex = v.tex;

			return out;
		} );

	return tverts;
}
std::vector<Triangle<x86>> Graphics::Traingulate( std::array<Vertex, 4> tverts )const
{
	auto make_tri = []( const Vertex& _a, const Vertex& _b, const Vertex& _c )
	{
		const Vertex* pa = &_a, *pb = &_b, *pc = &_c;
		if( pa->pos.y > pb->pos.y ) std::swap( pa, pb );
		if( pb->pos.y > pc->pos.y ) std::swap( pb, pc );
		if( pa->pos.y > pb->pos.y ) std::swap( pa, pb );

		std::vector<Triangle<x86>> tris;
		if( pa->pos.y == pb->pos.y )
		{
			if( ( *pa ).pos.x > ( *pb ).pos.x )
			{
				tris.emplace_back( *pb, *pa, *pc );
			}
			else
			{
				tris.emplace_back( *pa, *pb, *pc );
			}
		}
		else if( ( *pb ).pos.y == ( *pc ).pos.y )
		{
			if( ( *pb ).pos.x > ( *pc ).pos.x )
			{
				tris.emplace_back( *pa, *pc, *pb );
			}
			else
			{
				tris.emplace_back( *pa, *pb, *pc );
			}
		}
		else
		{
			const auto sHeight = ( *pb ).pos.y - ( *pa ).pos.y;
			const auto lHeight = ( *pc ).pos.y - ( *pa ).pos.y;
			const auto posSlope = ( ( *pc ).pos - ( *pa ).pos ) / lHeight;
			const auto dPos = ( *pa ).pos + ( posSlope * sHeight );

			const auto texSlope = ( ( *pc ).tex - ( *pa ).tex ) / lHeight;
			const auto dTex = ( *pa ).tex + ( texSlope * sHeight );
			const Vertex d = { dPos, dTex };

			if( d.pos.x < ( *pb ).pos.x )
			{
				// Flat bottom
				tris.emplace_back( *pa, d, ( *pb ) );
				// Flat top
				tris.emplace_back( d, *pb, *pc );
			}
			else
			{
				// Flat bottom
				tris.emplace_back( *pa, *pb, d );
				// Flat top
				tris.emplace_back( *pb, d, _c );
			}
		}

		return tris;
	};

	auto tris = make_tri( tverts[ 0 ], tverts[ 1 ], tverts[ 2 ] );
	auto t1 = make_tri( tverts[ 3 ], tverts[ 2 ], tverts[ 1 ] );
	tris.insert( tris.end(), t1.begin(), t1.end() );
	return tris;

	// Pack vertices into a Triangle object where the area will be calculated upon construction
	/*return{
		Triangle<x86>( tverts[ 0 ], tverts[ 1 ], tverts[ 2 ] ),
		Triangle<x86>( tverts[ 3 ], tverts[ 2 ], tverts[ 1 ] )
	};*/
}