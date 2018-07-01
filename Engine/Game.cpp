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
	gfx( wnd ),
	star( { 0.f, -200.f }, resources, wnd.kbd ),
	obs( { -32.f, 64.f }, { 32.f, -64.f } )
{
	obs += RectCenter( obs ) + Vec2f( 100.f, -200.f );
	std::vector<std::string> str = {"hello", "world"};

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
	const float dt = timer.Mark();
	star.Update( dt );

	const auto bb = star.GetRect();
	if( RectOverlaps( bb, obs ) )
	{
		const auto& pos = star.GetPosition();
		star.SetPosition( { pos.x - ( bb.right - obs.left ), pos.y } );
	}
}

void Game::ComposeFrame()
{
	const auto obsTrans =
		Matrix3x2f::Scaling( { RectWidth( obs ), RectHeight( obs ) } ) *
		Matrix3x2f::Translation( RectCenter( obs ) );

	gfx.DrawSprite( obsTrans, { resources.box } );

	star.Draw( gfx );
}
