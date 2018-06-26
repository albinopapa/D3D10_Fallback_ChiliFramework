#pragma once

#include "utility.h"
#include "Vector.h"

class BaryCoord
{
public:
	BaryCoord( float _a, float _b, float _c )
		:
		a( _a ), b( _b ), c( _c ),
		isbarycentric(
			is_in_range( a, 0.f, 1.f ) &&
			is_in_range( b, 0.f, 1.f ) &&
			is_in_range( c, 0.f, 1.f )
		)
	{}
	bool IsBarycentric()const
	{
		return isbarycentric;
	}
	Vec2f Interpolate( const Vec2f& v0, const Vec2f& v1, const Vec2f& v2 )const
	{
		return ( v0*a ) + ( v1*b ) + ( v2 * c );
	}

private:
	float a, b, c;
	bool isbarycentric = false;
};

struct Vertex
{
	Vec2f pos, tex;
};

class Triangle
{
public:
	Triangle() = default;
	Triangle( const Vertex& a, const Vertex& b, const Vertex& c )
		:
		v{ a, b, c },
		invarea( 1.f / ( CrossProduct( b.pos - a.pos, c.pos - a.pos ) * .5f ) )
	{}
	BaryCoord GetBaryCoords( const Vec2f& p )const
	{
		return {
			( CrossProduct( v[ 1 ].pos - p, v[ 2 ].pos - p ) * .5f ) * invarea,
			( CrossProduct( v[ 2 ].pos - p, v[ 0 ].pos - p ) * .5f ) * invarea,
			( CrossProduct( v[ 0 ].pos - p, v[ 1 ].pos - p ) * .5f ) * invarea
		};
	}
	const Vertex& GetVertex( int idx )const
	{
		assert( ( idx >= 0 && idx < 3 ) && "Idx out of range" );
		return v[ idx ];
	}
private:
	Vertex v[ 3 ];
	float invarea = 0.f;
};
