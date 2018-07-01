#include "Star.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "SpriteResources.h"

Star::Star( Vec2f _start_pos, SpriteResources& resource, Keyboard& _keyboard )
	:
	pKeyboard( &_keyboard ),
	resource( &resource ),
	pos( _start_pos )
{
	current = &resource.star_idle;
}
void Star::Update( float dt )
{
	float speed = 0.f;
	if( pKeyboard->KeyIsPressed( VK_LEFT ) || pKeyboard->KeyIsPressed( VK_RIGHT ) )
	{
		direction = pKeyboard->KeyIsPressed( VK_RIGHT ) ? 1.f : -1.f;
		current = &resource->star_walk;
		speed = 5.f;
	}
	else
	{
		current = &resource->star_idle;
	}

	pos.x += ( speed * direction );

	current->Advance( dt );
}
void Star::SetPosition( const Vec2f & _position )
{
	pos = _position;
}
Matrix<3, 2, float> Star::GetWorld()const
{
	return 
		Matrix<3, 2, float>::Scaling( Vec2f( current->GetFrame().width() * direction, current->GetFrame().height() ) ) *
		Matrix<3, 2, float>::Translation( pos );
}
RectF Star::GetRect()const
{
	const auto tl = Vec2f( -current->GetFrame().width(), current->GetFrame().height() ) * .5f;
	const auto br = -tl;
	return RectF( tl, br ) + pos;
}
const Vec2f & Star::GetPosition() const
{
	return pos;
}
void Star::Draw( Graphics& _gfx )const
{
	_gfx.DrawSprite( GetWorld(), current->GetFrame() );
}
