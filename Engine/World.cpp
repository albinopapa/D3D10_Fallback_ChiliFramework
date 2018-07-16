#include "World.h"
#include "Keyboard.h"
#include "Graphics.h"
#include <algorithm>
#include <fstream>
#include <random>

World::World( Keyboard& _keyboard )
	:
	star( { 0.f, -200.f }, resources, _keyboard )
{
	auto SaveLog = [ & ]()
	{
		std::ofstream file( "bench1.log", std::ios::app );
		std::string logInfo =
			"Frames rendered: " +
			std::to_string( counter ) +
			", in 5 seconds.\nAverage framerate: " +
			std::to_string( float( counter ) / 5.f ) + "\n";
		file << logInfo;
		file.close();
	};

	benchtimer = EventTimer<std::function<void()>>( 5.f, SaveLog );

	
}

void World::Update( float dt )
{
	benchtimer.Step();
	++counter;
	if( benchtimer.HasExpired() )
	{
		benchtimer.Reset();
		counter = 0;
	}

	star.Update( *this, dt );

	const auto bb = star.GetRect();
	Vec2f pos = star.GetPosition();
	Vec2f vel = star.GetVelocity();
	
	if( IsOnGround( bb ) )
	{
		vel.y = 0.f;
		pos.y += ( ground - bb.bottom );
	}

	star.SetPosition( pos );
	star.SetVelocity( vel );
}

#include "Mouse.h"
void World::Render( Graphics& _graphics )const
{
	constexpr float halfWidth = float( Graphics::ScreenWidth / 2 );
	constexpr float halfHeight = float( Graphics::ScreenHeight / 2 );
	const RectF scrRect = static_cast< RectF >( _graphics.GetScreenRect() ) + Vec2f( -halfWidth, -halfHeight );
	
	const float scale = 1.f;
	const auto bg_trans =
		Matrix3x2f::Rotation( 0.f * ( 3.141592f / 180.f ) ) * 
		Matrix3x2f::Scaling( { float( Graphics::ScreenWidth )* scale, float( Graphics::ScreenHeight ) * scale } );

	_graphics.DrawSprite( bg_trans, { resources.bg },
		[ &_graphics ]( int _x, int _y, Color _c )
		{
			_graphics.PutPixel( _x, _y, _c );
		} );

	std::for_each(obstacles.begin(),obstacles.end(),
		[ &_graphics, this, &scrRect ]( const RectF& obs )
		{
			const auto obsCenter = RectCenter( obs );
			const auto obsTrans =
				Matrix3x2f::Scaling( { RectWidth( obs ), RectHeight( obs ) } ) *
				Matrix3x2f::Translation( obsCenter );
			
			if( RectContains( scrRect, obsCenter ) )
			{
				_graphics.DrawSprite( obsTrans, { resources.box }, [ &_graphics ]( int X, int Y, Color C )
					{
						_graphics.PutPixel( X, Y, C );
					} );
			}
		} );

	star.Draw( _graphics );

}

std::optional<RectF> World::CollidesWithObject( const RectF& _left )
{
	auto findit = std::find_if(obstacles.begin(),obstacles.end(),
		[ &_left ]( const RectF& _right )
		{
			return RectOverlaps( _left, _right );
		} );
	
	return findit != obstacles.end() ?
		std::optional<RectF>( *findit ) : std::optional<RectF>();
}

bool World::IsOnGround( const RectF& _bb )const
{
	return _bb.bottom < ground;
}