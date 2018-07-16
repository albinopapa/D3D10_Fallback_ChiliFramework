#include "StarState.h"
#include "Star.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "SpriteResources.h"
#include "World.h"


StarIdle::StarIdle( class Star& _star, SpriteResources& _resources )
	:
	star( _star ),
	resources( _resources )
{}

void StarIdle::Update( World& _world, Keyboard& _keyboard, float dt )
{
	acceleration = { 0.f, 0.f };
	if( _keyboard.KeyIsPressed( VK_LEFT ) || _keyboard.KeyIsPressed( VK_RIGHT ) )
	{
		if( _keyboard.KeyIsPressed( VK_RIGHT ) )
		{
			acceleration.x = Star::maxSpeed;
			star.direction = 1.f;
		}
		else if( _keyboard.KeyIsPressed( VK_LEFT ) )
		{
			acceleration.x = -Star::maxSpeed;
			star.direction = -1.f;
		}

		star.SetNextState( std::make_unique<StarWalk>( star, resources ) );
	}
	else if( _keyboard.KeyIsPressed( VK_SPACE ) )
	{
		acceleration.y = Star::jumpAccel;
		star.SetNextState( std::make_unique<StarJump>( star, resources ) );
	}
}

void StarIdle::Draw( Graphics& _graphics )
{
	_graphics.DrawSprite( star.GetWorld(), { resources.star_idle.GetFrame() },
		[ &_graphics ]( int X, int Y, Color C )
		{
			_graphics.PutPixelAlpha( X, Y, C );
		} );
}

const Vec2f& StarIdle::GetAcceleration()const
{
	return acceleration;
}


StarWalk::StarWalk( Star& _star, SpriteResources& _resources )
	:
	star( _star ),
	resources( _resources )
{}

void StarWalk::Update( World& _world, Keyboard& _keyboard, float dt )
{
	acceleration = { 0.f, 0.f };
	bool moveKeyPressed = false;
	if( _keyboard.KeyIsPressed( VK_LEFT ) || _keyboard.KeyIsPressed( VK_RIGHT ) )
	{
		if( _keyboard.KeyIsPressed( VK_RIGHT ) )
		{
			acceleration.x = Star::maxSpeed;
			star.direction = 1.f;
		}
		else
		{
			acceleration.x = -Star::maxSpeed;
			star.direction = -1.f;
		}

		moveKeyPressed = true;
	}
	if( _keyboard.KeyIsPressed( VK_SPACE ) )
	{
		acceleration.y = Star::jumpAccel;
		star.SetNextState( std::make_unique<StarJump>( star, resources ) );
		moveKeyPressed = true;
	}
	
	if( star.vel.x == 0.f && !moveKeyPressed )
	{
		star.SetNextState( std::make_unique<StarIdle>( star, resources ) );
	}

	resources.star_walk.Advance( dt );
}

void StarWalk::Draw( Graphics& _graphics )
{
	_graphics.DrawSprite( star.GetWorld(), { resources.star_walk.GetFrame() },
		[ &_graphics ]( int X, int Y, Color C )
		{
			_graphics.PutPixelAlpha( X, Y, C );
		} );
}

const Vec2f& StarWalk::GetAcceleration()const
{
	return acceleration;
}


StarJump::StarJump( Star& _star, SpriteResources& _resources )
	:
	star( _star ),
	resources( _resources )
{}

void StarJump::Update( World& _world, Keyboard& _keyboard, float dt )
{
	acceleration = { 0.f, 0.f };
	if( _world.IsOnGround( star.GetRect() ) )
	{
		if( star.vel.x == 0.f )
		{
			star.SetNextState( std::make_unique<StarIdle>( star, resources ) );
		}
		else
		{
			star.SetNextState( std::make_unique<StarWalk>( star, resources ) );
		}
	}
	if( _keyboard.KeyIsPressed( VK_LEFT ) || _keyboard.KeyIsPressed( VK_RIGHT ) )
	{
		if( _keyboard.KeyIsPressed( VK_RIGHT ) )
		{
			acceleration.x = Star::maxSpeed;
			star.direction = 1.f;
		}
		else
		{
			acceleration.x = -Star::maxSpeed;
			star.direction = -1.f;
		}
	}
	
	resources.star_walk.Advance( dt );
}

void StarJump::Draw( Graphics& _graphics )
{
	_graphics.DrawSprite( star.GetWorld(), { resources.star_walk.GetFrame() },
		[ &_graphics ]( int X, int Y, Color C )
		{
			_graphics.PutPixelAlpha( X, Y, C );
		} );
}

const Vec2f& StarJump::GetAcceleration()const
{
	return acceleration;
}
