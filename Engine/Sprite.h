#pragma once

#include "Rect.h"
#include "image.h"



class Sprite
{
public:
	Sprite() = default;
	Sprite( const grafix::image& _img );
	Rect<float> BoundingRect( float _xoff, float _yoff )const;
	Rect<float> BoundingRect( const Vec2f& _offset )const;
	uint32_t pixel( int _x, int _y )const;
	class uint128 pixels( int _x, int _y )const;
	float width()const;
	float height()const;

private:
	const grafix::image* img;
};

