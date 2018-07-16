#pragma once

#include "simd.h"
#include "Vector.h"

using Vec2SSE = Vector<2, float128>;

inline Vec2SSE _vectorcall operator*( const Vec2SSE v, const float128 s )
{
	return { v.x * s, v.y * s };
}

inline float128 _vectorcall CrossProduct( const Vec2SSE left, const Vec2SSE right )
{
	return ( left.y * right.x ) - ( left.x * right.y );
}