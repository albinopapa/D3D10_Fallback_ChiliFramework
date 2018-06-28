#pragma once

#include "Rect.h"
#include <cassert>

template<class Fn>
void for_each( int _xStart, int _yStart, int _xEnd, int _yEnd, Fn _func )
{
	for( int y = _yStart; y < _yEnd; ++y )
	{
		for( int x = _xStart; x < _xEnd; ++x )
		{
			_func( x, y );
		}
	}
}

template<class Fn>
void for_each( const Rect<int>& _region, Fn _func )
{
	for( int y = _region.top; y < _region.bottom; ++y )
	{
		for( int x = _region.left; x < _region.right; ++x )
		{
			_func( x, y );
		}
	}
}

template<size_t NumElements, class SimdFn, class x86Fn>
void simd_for_each( int _xStart, int _yStart, int _xEnd, int _yEnd, SimdFn _simd_func, x86Fn _x86_func )
{
	static_assert( NumElements % 4 == 0,
		"SimdWidth must be multiple of 4 ( sse width = 4, avx256 width = 8, avx512 width = 16 )" );
	
	constexpr auto simd_mask = int( NumElements - 1 );

	const auto mxStart = ( _xStart + simd_mask ) & ( ~simd_mask );
	const auto mxEnd = _xEnd & ( ~simd_mask );

	for( int y = _yStart; y < _yEnd; ++y )
	{
		// Pre-process to aligned memory
		for( int x = _xStart; x < mxStart; ++x )
		{
			_x86_func( x, y );
		}
		// Process to end of alignment
		for( int x = mxStart; x < mxEnd; x += NumElements )
		{
			_simd_func( x, y );
		}
		// Post-process remaining
		for( int x = mxEnd; x < _xEnd; ++x )
		{
			_x86_func( x, y );
		}
	}
}

template<class T, class Pred>
void swap_if( T& _a, T& _b, Pred _pred )
{
	if( _pred( _a, _b ) )
	{
		std::swap( _a, _b );
	}
};

template<class T>
bool is_in_range( const T& val, const T& low, const T& hi )
{
	return ( ( val >= low ) && ( val <= hi ) );
}

inline Vec2f reflect( const Vec2f& d, const Vec2f& n )
{
	return d + ( -2.f * ( DotProduct( d, n ) * n ) );
}