#include "Sprite.h"
#include "simd.h"

Sprite::Sprite( const grafix::image& _img )
	:
	img( &_img )
{}

RectF Sprite::BoundingRect( float _xoff, float _yoff )const
{
	return BoundingRect( { _xoff, _yoff } );
}

RectF Sprite::BoundingRect( const Vec2f& _offset )const
{
	return ( RectF( 0.f, 0.f, float( img->width() ), float( img->height() ) ) + _offset );
}

uint32_t Sprite::pixel( int _x, int _y )const
{
	return ( *img )( _x, _y );
}

uint128 Sprite::pixels( int _x, int _y )const
{
	return uint128( &( *img )( _x, _y ) );
}

float Sprite::width()const
{
	return float( img->width() );
}

float Sprite::height()const
{
	return float( img->height() );
}
