#pragma once

#include "simd_math.h"
#include "utility.h"

struct x86 {};
struct SSE {};

template<class T> class BaryCoord {};
template<class T>class Triangle {};

struct Vertex
{
	Vec2f pos, tex;
};

template<> class BaryCoord<x86>
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

template<> class Triangle<x86>
{
public:
	Triangle() = default;
	Triangle( const Vertex& a, const Vertex& b, const Vertex& c )
		:
		v{ a, b, c },
		invarea( 1.f / ( CrossProduct( b.pos - a.pos, c.pos - a.pos ) * .5f ) )
	{}
	BaryCoord<x86> GetBaryCoords( const Vec2f& p )const
	{
		const Vec2f v0p = v[0].pos - p;
		const Vec2f v1p = v[1].pos - p;
		const Vec2f v2p = v[2].pos - p;

		const float a = ( CrossProduct( v1p, v2p ) * .5f ) * invarea;
		const float b = ( CrossProduct( v2p, v0p ) * .5f ) * invarea;
		const float c = ( CrossProduct( v0p, v1p ) * .5f ) * invarea;

		return { a, b, c };
	}
	const Vertex& operator[]( int idx )const
	{
		return GetVertex( idx );
	}
	const Vertex& GetVertex( int idx )const
	{
		assert( ( idx >= 0 && idx < 3 ) && "Idx out of range" );
		return v[ idx ];
	}
	float GetInverseArea()const
	{
		return invarea;
	}
private:
	Vertex v[ 3 ];
	float invarea = 0.f;
};

template<> class BaryCoord<SSE>
{
public:
	BaryCoord( float128 _a, float128 _b, float128 _c )
		:
		a( _a ), b( _b ), c( _c )		
	{
		mask =
			( ( a >= simd_traits<float128>::zero ) & ( a <= simd_traits<float128>::one ) ) &
			( ( b >= simd_traits<float128>::zero ) & ( b <= simd_traits<float128>::one ) ) &
			( ( c >= simd_traits<float128>::zero ) & ( c <= simd_traits<float128>::one ) );

		isbarycentric = _mm_movemask_ps( mask );
	}
	bool IsBarycentric( int idx )const
	{
		return ( isbarycentric >> ( 3 - idx ) ) & 1;
	}
	Vec2SSE _vectorcall Interpolate( const Vec2SSE v0, const Vec2SSE v1, const Vec2SSE v2 )const
	{	
		return ( v0 * a ) + ( v1 * b ) + ( v2 * c );
	}

private:
	float128 a, b, c, mask;
	int isbarycentric;

};

template<> class Triangle<SSE>
{
public:
	Triangle( const Triangle<x86>& tri )
		:
		invarea( tri.GetInverseArea() )
	{
		auto temp0 = float128( reinterpret_cast< const float* >( &tri.GetVertex( 0 ) ) );
		auto temp1 = float128( reinterpret_cast< const float* >( &tri.GetVertex( 1 ) ) );
		auto temp2 = float128( reinterpret_cast< const float* >( &tri.GetVertex( 2 ) ) );

		vp[ 0 ].x = shuffle<0, 0, 0, 0>( temp0, temp0 );
		vp[ 0 ].y = shuffle<1, 1, 1, 1>( temp0, temp0 );
		vt[ 0 ].x = shuffle<2, 2, 2, 2>( temp0, temp0 );
		vt[ 0 ].y = shuffle<3, 3, 3, 3>( temp0, temp0 );

		vp[ 1 ].x = shuffle<0, 0, 0, 0>( temp1, temp1 );
		vp[ 1 ].y = shuffle<1, 1, 1, 1>( temp1, temp1 );
		vt[ 1 ].x = shuffle<2, 2, 2, 2>( temp1, temp1 );
		vt[ 1 ].y = shuffle<3, 3, 3, 3>( temp1, temp1 );
		
		vp[ 2 ].x = shuffle<0, 0, 0, 0>( temp2, temp2 );
		vp[ 2 ].y = shuffle<1, 1, 1, 1>( temp2, temp2 );
		vt[ 2 ].x = shuffle<2, 2, 2, 2>( temp2, temp2 );
		vt[ 2 ].y = shuffle<3, 3, 3, 3>( temp2, temp2 );
	}

	BaryCoord<SSE> _vectorcall GetBaryCoords( const Vec2SSE P )const
	{
		const Vec2SSE v0p = vp[ 0 ] - P;
		const Vec2SSE v1p = vp[ 1 ] - P;
		const Vec2SSE v2p = vp[ 2 ] - P;

		const float128 a = CrossProduct( v1p, v2p ) * simd_traits<float128>::half * invarea;
		const float128 b = CrossProduct( v2p, v0p ) * simd_traits<float128>::half * invarea;
		const float128 c = CrossProduct( v0p, v1p ) * simd_traits<float128>::half * invarea;

		return { a, b, c };
	}
	Vec2SSE GetVertexPosition( int idx )
	{
		return vp[ idx ];
	}
	Vec2SSE GetVertexTexcoord( int idx )const
	{
		return vt[ idx ];
	}

private:
	Vec2SSE vp[ 3 ], vt[ 3 ];
	float128 invarea;
};