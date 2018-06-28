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
	idle( "sprite/idle", 1 ),
	walk( "sprite/walk", 30 ),
	pos( 0.f, -200.f )
{
	const auto sprite = idle.GetFrame();
	transform = Matrix<3, 2, float>::Scaling( { sprite.width(), sprite.height() } );
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
	float speed = 0.f;
	if( wnd.kbd.KeyIsPressed( VK_LEFT ) || wnd.kbd.KeyIsPressed( VK_RIGHT ) )
	{
		direction = wnd.kbd.KeyIsPressed( VK_RIGHT ) ? 1.f : -1.f;
		current = &walk;
		speed = 5.f;
	}
	else
	{
		current = &idle;
	}
	
	pos.x += ( speed * direction );

	current->Advance( 0.016f );
	transform = 
		Matrix<3, 2, float>::Scaling( Vec2f( current->GetFrame().width() * direction, current->GetFrame().height() ) * 1.f ) *
		Matrix<3, 2, float>::Translation( pos );
}

void Game::ComposeFrame()
{
	gfx.DrawSprite( transform, current->GetFrame() );
}
