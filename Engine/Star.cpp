#include "Star.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "SpriteResources.h"

Star::Star( Vec2f _start_pos, SpriteResources& resource, Keyboard& _keyboard )
	:
	pKeyboard( &_keyboard ),
	resource( &resource ),
	pos( _start_pos ),
	vel( 0.f, 0.f ),
	state( std::make_unique<StarIdle>( *this, resource ) )
{
	
}

void Star::Update( World& _world, float dt )
{
	if( next )
		state = std::move( next );

	state->Update( _world, *pKeyboard, dt );
	
	Vec2f accel = state->GetAcceleration();
	accel.y -= 100.f;

	vel += accel;
	vel *= .6f;

	vel = { 
		( vel.x > 0.f ) ? std::min( vel.x, maxSpeed ) : std::max( vel.x, -maxSpeed ),
		( vel.y > 0.f ) ? std::min( vel.y, jumpAccel ) : std::max( vel.y, -jumpAccel )
	};

	vel = {
		std::fabs( vel.x ) <= .001f ? 0.f : vel.x,
		std::fabs( vel.y ) <= .001f ? 0.f : vel.y
	};

	pos += ( vel * dt );
}

void Star::SetPosition( const Vec2f & _position )
{
	pos = _position;
}

void Star::SetVelocity( const Vec2f& _velocity )
{
	vel = _velocity;
}

Matrix<3, 2, float> Star::GetWorld()const
{
	return 
		Matrix<3, 2, float>::Scaling( Vec2f( width * direction, height ) ) *
		Matrix<3, 2, float>::Translation( pos );
}

RectF Star::GetRect()const
{
	const auto tl = Vec2f( -width, height ) * .5f;
	const auto br = -tl;
	return RectF( tl, br ) + pos;
}

const Vec2f & Star::GetPosition() const
{
	return pos;
}

const Vec2f& Star::GetVelocity()const
{
	return vel;
}

void Star::Draw( Graphics& _gfx )const
{
	state->Draw( _gfx );
}

void Star::SetNextState( std::unique_ptr<StarState> _state )
{
	next = std::move( _state );
}

