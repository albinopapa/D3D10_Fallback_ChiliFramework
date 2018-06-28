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
#include <memory>
#include <array>

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
			// Clamp bounding box to screen dimensions
			return {
				std::max( bb.Box().left, clipRect.left ),
				std::max( bb.Box().top, clipRect.top ),
				std::min( bb.Box().right, clipRect.right ),
				std::min( bb.Box().bottom, clipRect.bottom )
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
	void DrawSprite( const Matrix<3, 2, float>& transform, Sprite sprite );

private:
	void DrawTriangle( const Matrix<3, 2, float>& transform, Sprite sprite );
	Rect<int> GetScreenRect()const;

public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;

private:
	std::array<Vertex,4> quad;
	std::unique_ptr<D3DBackend> backend;
	Color * pSysBuffer = nullptr;
	const Matrix<3, 2, float> screen;
};