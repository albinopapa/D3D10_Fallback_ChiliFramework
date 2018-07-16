/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
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
#pragma once

#include "ChiliWin.h"
#include "Colors.h"
#include "D3DBackend.h"
#include "Matrix.h"
#include "Rect.h"
#include "Sprite.h"
#include "Triangle.h"
#include <algorithm>
#include <array>
#include <memory>
#include <vector>

class Graphics
{
	class BoundingBox
	{
	public:		
		BoundingBox( const std::array<Vertex, 4>& tverts )
			:
			box( std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
				 std::numeric_limits<int>::min(), std::numeric_limits<int>::min() )
		{
			std::for_each( tverts.begin(), tverts.end(), [ & ]( const Vertex& v )
				{
					box.left	= std::min( int( std::floorf( v.pos.x ) ), box.left );
					box.top		= std::min( int( std::floorf( v.pos.y ) ), box.top );
					box.right   = std::max( int( std::ceilf( v.pos.x ) - 1.f ), box.right );
					box.bottom  = std::max( int( std::ceilf( v.pos.y ) - 1.f ), box.bottom );
				} );
		}
		const Rect<int>& Box()const
		{
			return box;
		}
	private:
		Rect<int> box;
	};
	class Clipper
	{
	public:
		Rect<int> operator()( const BoundingBox& bb, const Rect<int>& clipRect )const
		{
			const auto box = bb.Box();

			// Clamp bounding box to screen dimensions
			return {
				std::clamp( box.left, clipRect.left, clipRect.right ),
				std::clamp( box.top, clipRect.top, clipRect.bottom ),
				std::clamp( box.right, clipRect.left, clipRect.right ),
				std::clamp( box.bottom, clipRect.top, clipRect.bottom )
			};
		}
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	~Graphics();

	Graphics& operator=( const Graphics& ) = delete;

	void EndFrame();
	void BeginFrame();

	void PutPixel( int x, int y, int r, int g, int b );
	void PutPixel( int x, int y, Color c );
	Color GetPixel( int x, int y )const;
	void PutPixelAlpha( int x, int y, Color c );

	void DrawLine( const Vec2f& p0, const Vec2f& p1, const float thickness, const Sprite& sprite );

	template<class PixelFn>
	void DrawSprite( const Matrix<3, 2, float>& transform, Sprite sprite, PixelFn pfn )
	{
		DrawTriangle( transform, sprite, pfn );
	}
	Rect<int> GetScreenRect()const;

public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;

private:
	std::array<Vertex, 4> TransformVertices( const Matrix<3, 2, float>& transform )const;
	std::vector<Triangle<x86>> Traingulate( std::array<Vertex, 4> vertices )const;

	template<class PixelFn>
	void x86Rasterize( const int X, const int Y, const Vec2f& P, const Triangle<x86>& tri, const Sprite& sprite, PixelFn pfn )
	{
		const auto coord = tri.GetBaryCoords( P );
		if( coord.IsBarycentric() )
		{
			const auto tc = coord.Interpolate( tri[ 0 ].tex, tri[ 1 ].tex, tri[ 2 ].tex );

			const int tx = int( tc.x * ( sprite.width() - 1.f ) );
			const int ty = int( tc.y * ( sprite.height() - 1.f ) );

			pfn( X, Y, sprite.pixel( tx, ty ) );
		}
	}

	template<class PixelFn>
	void DrawTriangle( const Matrix<3, 2, float>& transform, const Sprite& sprite, PixelFn pfn )
	{
		auto SortVertices = []( Vertex a, Vertex b, Vertex c )
		{
			if( a.pos.y > b.pos.y ) std::swap( a, b );
			if( b.pos.y > c.pos.y ) std::swap( b, c );
			if( a.pos.y > b.pos.y ) std::swap( a, b );

			return Triangle<x86>( a, b, c );
		};

		auto SortTriangle = []( const Triangle<x86>& tri, std::vector<Triangle<x86>>& fBot, std::vector<Triangle<x86>>& fTop )
		{
			if( tri[ 0 ].pos.y == tri[ 1 ].pos.y )
			{
				if( tri[ 0 ].pos.x > tri[ 1 ].pos.x )
				{
					fTop.emplace_back( tri[ 1 ], tri[ 0 ], tri[ 2 ] );
				}
				else
				{
					fTop.emplace_back( tri );
				}
			}
			else if( tri[ 1 ].pos.y == tri[ 2 ].pos.y )
			{
				if( tri[ 1 ].pos.x > tri[ 2 ].pos.x )
				{
					fBot.emplace_back( tri[ 0 ], tri[ 2 ], tri[ 1 ] );
				}
				else
				{
					fBot.emplace_back( tri );
				}
			}
			else
			{
				const auto sHeight = tri[ 1 ].pos.y - tri[ 0 ].pos.y;
				const auto lHeight = tri[ 2 ].pos.y - tri[ 0 ].pos.y;
				const auto posSlope = ( tri[ 2 ].pos - tri[ 0 ].pos ) / lHeight;
				const auto dPos = tri[ 0 ].pos + ( posSlope * sHeight );

				const auto texSlope = ( tri[ 2 ].tex - tri[ 0 ].tex ) / lHeight;
				const auto dTex = tri[ 0 ].tex + ( texSlope * sHeight );
				const Vertex d = { dPos, dTex };

				if( d.pos.x < tri[ 1 ].pos.x )
				{
					// Flat bottom
					fBot.emplace_back( tri[ 0 ], d, tri[ 1 ] );
					// Flat top
					fTop.emplace_back( d, tri[ 1 ], tri[ 2 ] );
				}
				else
				{
					// Flat bottom
					fBot.emplace_back( tri[ 0 ], tri[ 1 ], d );
					// Flat top
					fTop.emplace_back( tri[ 1 ], d, tri[ 2 ] );
				}
			}

		};
		
		auto Rasterize = [ this, &sprite, &pfn ]( const int x, const int y, const Triangle<x86>& tri )
		{
			const auto coord = tri.GetBaryCoords( { float( x ), float( y ) } );

			const auto tex = coord.Interpolate( tri[ 0 ].tex, tri[ 1 ].tex, tri[ 2 ].tex );

			const float clampWidth = ( sprite.width() - 1.f );
			const float clampHeight = ( sprite.height() - 1.f );
			
			const auto itx = int( tex.x * clampWidth );
			const auto ity = int( tex.y * clampHeight );

			pfn( x, y, sprite.pixel( itx, ity ) );
		};

		auto DrawFlatBottom = [ &Rasterize ]( const Triangle<x86>& tri )
		{
			const int yStart = int( std::max( 0.f, ( std::ceilf( tri[ 0 ].pos.y ) ) ) );
			const int yEnd = int( std::min( float( ScreenHeight ), ( std::floorf( tri[ 2 ].pos.y ) ) ) );

			const auto height = 1.f / ( tri[ 2 ].pos.y - tri[ 0 ].pos.y );

			const auto lEdge = tri[ 1 ].pos - tri[ 0 ].pos;
			const auto rEdge = tri[ 2 ].pos - tri[ 0 ].pos;

			const auto left  = lEdge.x * height;
			const auto right = rEdge.x * height;

			constexpr float clipX = float( ScreenWidth );

			auto xIteration = [ &tri, &left, &right, &Rasterize, clipX ]( const float _y )
			{
				const float yOffset = _y - tri[ 0 ].pos.y;

				const float fxStart = tri[ 0 ].pos.x + ( yOffset * left );
				const int xStart = int( std::max( 0.f, fxStart + .5f ) );

				const float fxEnd = tri[ 0 ].pos.x + ( yOffset * right );
				const int xEnd = int( std::min( clipX, fxEnd ) );

				for( int x = xStart; x < xEnd; ++x )
				{
					Rasterize( x, int( _y ), tri );
				}
			};

			const int yStepStart = ( yStart + 3 )& ~3;
			const int yStepEnd = yEnd & ~3;

			for( int y = yStart; y < yStepStart; ++y )
			{
				xIteration( y );
			}
			for( int y = yStepStart; y < yStepEnd; y += 4 )
			{
				const float y0 = float( y + 0 );
				const float y1 = float( y + 1 );
				const float y2 = float( y + 2 );
				const float y3 = float( y + 3 );

				xIteration( y0 );
				xIteration( y1 );
				xIteration( y2 );
				xIteration( y3 );
			}
			for( int y = yStepEnd; y < yEnd; ++y )
			{
				xIteration( y );
			}
		};

		auto DrawFlatTop = [ &Rasterize ]( const Triangle<x86>& tri )
		{
			const int yStart = int( std::max( 0.f, ( std::ceilf( tri[ 0 ].pos.y ) ) ) );
			const int yEnd = int( std::min( float( ScreenHeight ), ( std::floorf( tri[ 2 ].pos.y ) ) ) );

			const auto height = 1.f / ( tri[ 2 ].pos.y - tri[ 0 ].pos.y );

			const auto lEdge = tri[ 2 ].pos - tri[ 0 ].pos;
			const auto rEdge = tri[ 2 ].pos - tri[ 1 ].pos;

			const float left  = lEdge.x * height;
			const float right = rEdge.x * height;

			constexpr float clipX = float( ScreenWidth );

			auto xIteration = [ &tri, &left, &right, &Rasterize, clipX ]( const float _y )
			{
				const float yOffset = _y - tri[ 0 ].pos.y;

				const auto fxStart = tri[ 0 ].pos.x + ( yOffset * left );
				const int xStart = int( std::max( 0.f, fxStart + .5f ) );

				const auto fxEnd = tri[ 1 ].pos.x + ( yOffset * right );
				const int xEnd = int( std::min( clipX, fxEnd ) );

				for( int x = xStart; x < xEnd; ++x )
				{
					Rasterize( x, int( _y ), tri );
				}
			};

			const int yStepStart = ( yStart + 3 )& ~3;
			const int yStepEnd = yEnd & ~3;

			for( int y = yStart; y < yStepStart; ++y )
			{
				xIteration( y );
			}
			for( int y = yStepStart; y < yStepEnd; y += 4 )
			{
				const float y0 = float( y + 0 );
				const float y1 = float( y + 1 );
				const float y2 = float( y + 2 );
				const float y3 = float( y + 3 );

				xIteration( y0 );
				xIteration( y1 );
				xIteration( y2 );
				xIteration( y3 );
			}
			for( int y = yStepEnd; y < yEnd; ++y)
			{
				xIteration( y );
			}

		};

		const auto tverts = TransformVertices( transform );

		std::array<Triangle<x86>, 2> tris = {
			SortVertices( tverts[ 0 ], tverts[ 1 ], tverts[ 2 ] ),
			SortVertices( tverts[ 3 ], tverts[ 2 ], tverts[ 1 ] )
		};

		std::vector<Triangle<x86>> fBottoms, fTops;
		fBottoms.reserve( 2 );
		fTops.reserve( 2 );

		for( const auto& tri : tris )
		{
			SortTriangle( tri, fBottoms, fTops );
		}

		// Flat bottom
		for( const auto& tri : fBottoms )
		{
			DrawFlatBottom( tri );
		}

		// Flat top
		for( const auto& tri : fTops )
		{
			DrawFlatTop( tri );
		}
		//std::vector<Triangle<SSE>> mTris;
		//mTris.reserve( tris.size() );
		//
		//std::transform( tris.begin(), tris.end(), std::back_inserter( mTris ), []( const Triangle<x86>& _a )
		//	{
		//		return Triangle<SSE>( _a );
		//	} );
		//
		//const float128 xIndices = { 3.f, 2.f, 1.f, 0.f };
		//
		//// Get texture dimensions
		//float128 texWidth( sprite.width() - 1.f );
		//float128 texHeight( sprite.height() - 1.f );
		//
		//
		//auto x86RasterSetup = [ this, tris, pfn, &sprite ]( int ix, int iy )
		//{
		//	const Vec2f p = Vec2f( float( ix ), float( iy ) );
		//	x86Rasterize( ix, iy, p, tris[ 0 ], sprite, pfn );
		//	x86Rasterize( ix, iy, p, tris[ 1 ], sprite, pfn );
		//};
		//auto sseRasterSetup = [ this, mTris, pfn, &sprite, &xIndices, &texWidth, &texHeight ]( int ix, int iy )
		//{
		//	Vec2SSE mp( float128( ix ) + xIndices, float128( float( iy ) ) );
		//	
		//	for( int i = 0; i < 2; ++i )
		//	{
		//		const auto& tri = mTris[ i ];
		//
		//		const auto coords = tri.GetBaryCoords( mp );
		//
		//		const auto v0 = tri.GetVertexTexcoord( 0 );
		//		const auto v1 = tri.GetVertexTexcoord( 1 );
		//		const auto v2 = tri.GetVertexTexcoord( 2 );
		//
		//		// Interpolate texture coordinates
		//		Vec2SSE tc = coords.Interpolate(
		//			tri.GetVertexTexcoord( 0 ),
		//			tri.GetVertexTexcoord( 1 ),
		//			tri.GetVertexTexcoord( 2 )
		//		);
		//
		//		tc.x *= texWidth;
		//		tc.y *= texHeight;
		//
		//		// Convert to integers
		//		int128 itx = int128( tc.x );
		//		int128 ity = int128( tc.y );
		//
		//		// Store results
		//		alignas( 16 ) int tcx[ 4 ], tcy[ 4 ];
		//		itx.Store( tcx );
		//		ity.Store( tcy );
		//
		//		// Loop through results, skipping any that were masked out
		//		for( int i = 0; i < 4; ++i )
		//		{
		//			const auto xPos = ix + i;
		//			const auto mask = ( ~coords.IsBarycentric( i ) + 1 );
		//			const Color color =
		//				( mask & sprite.pixel( tcx[ i ], tcy[ i ] ) ) |
		//				( ~mask & GetPixel( xPos, iy ).dword );
		//
		//			pfn( xPos, iy, color );
		//		}
		//	}
		//	//sseRasterize( ix, iy, mp, mTris[ 0 ], sprite, pfn );
		//	//sseRasterize( ix, iy, mp, mTris[ 1 ], sprite, pfn );
		//};
		//
		// Loop through bounding box
		//const auto box = BoundingBox( tverts );
		//const auto rect = Clipper()( box, GetScreenRect() );
		//
		//simd_for_each<4>( rect.left, rect.top, rect.right, rect.bottom, sseRasterSetup, x86RasterSetup );
		//for_each( rect, x86RasterSetup );
	}
private:
	std::array<Vertex,4> quad;
	std::unique_ptr<D3DBackend> backend;
	Color * pSysBuffer = nullptr;
	const Matrix<3, 2, float> screen;
};