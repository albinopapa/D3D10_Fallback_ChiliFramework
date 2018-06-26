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
#include <algorithm>
#include <memory>

class Graphics
{
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	~Graphics();

	Graphics& operator=( const Graphics& ) = delete;

	void EndFrame();
	void BeginFrame();

	void PutPixel( int x, int y, int r, int g, int b );
	void PutPixel( int x,int y,Color c );
public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;

private:
	std::unique_ptr<D3DBackend> backend;
	Color * pSysBuffer = nullptr;
};