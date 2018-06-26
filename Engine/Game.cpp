/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
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
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
}

void Game::ComposeFrame()
{
	// D3D 10 only supports RGBA colors, so here's a little test
	// The top left should be red, top right should be blue
	// The middle should be green
	// The bottom left should be blue, bottom right should be red
	for( int y = 0; y < Graphics::ScreenHeight; ++y )
	{
		for( int x = 0; x < Graphics::ScreenWidth; ++x )
		{
			Color c;
			if( y < 200 )
			{
				c = ( x < Graphics::ScreenWidth / 2 ) ? Colors::Red : Colors::Blue;
			}
			else if( y < 400 )
			{
				c = Colors::Green;
			}
			else
			{
				c = ( x < Graphics::ScreenWidth / 2 ) ? Colors::Blue : Colors::Red;
			}
			

			gfx.PutPixel( x, y, c );
		}
	}


}
