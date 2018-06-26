#pragma once

#include <algorithm>
#include <cassert>

template<class T> T operator+( const T& _src, const T& _other )
{
	return T( _src ) += _other;
}
template<class T> T operator-( const T& _lhs )
{
	return T{} -= _lhs;
}
template<class T> T operator-( const T& _src, const T& _other )
{
	return T( _src ) -= _other;
}
template<class T> T operator*( const T& _src, const float _other )
{
	return T( _src ) *= _other;
}
template<class T> T operator*( const float _src, const T& _other )
{
	return T( _other ) *= _src;
}
template<class T> T operator/( const T& _src, const float _other )
{
	return T( _src ) /= _other;
}


template<template<size_t, class> class vec_type, size_t ElemCount, class scalar_type>
scalar_type DotProduct( const vec_type<ElemCount, scalar_type>& _lhs, const vec_type<ElemCount, scalar_type>& _rhs )
{
	scalar_type result = ( _lhs.x * _rhs.x ) + ( _lhs.y * _rhs.y );
	if constexpr( vec_type<ElemCount, scalar_type>::elem_count == 3 )
	{
		result += ( _lhs.z * _rhs.z );
	}

	return result;
}

template<template<size_t, class> class vec_type, size_t ElemCount, class scalar_type>
auto CrossProduct( const vec_type<ElemCount, scalar_type>& _lhs, const vec_type<ElemCount, scalar_type>& _rhs )
->std::conditional_t<ElemCount == 2, scalar_type, vec_type<ElemCount, scalar_type>>
{
	if constexpr( ElemCount == 2 )
	{
		return ( _lhs.y * _rhs.x ) - ( _lhs.x * _rhs.y );
	}
	else if constexpr( ElemCount == 3 )
	{
		return
			vec_type<ElemCount, scalar_type>( _lhs.y * _rhs.z, _lhs.z * _rhs.x, _lhs.x * _rhs.y ) -
			vec_type<ElemCount, scalar_type>( _lhs.z * _rhs.y, _lhs.x * _rhs.z, _lhs.y * _rhs.z );
	}
}

template<template<size_t, class> class vec_type, size_t ElemCount, class scalar_type>
vec_type<ElemCount, scalar_type> Lerp( const vec_type<ElemCount, scalar_type>& _start, const vec_type<ElemCount, scalar_type>& _end, scalar_type _t )
{
	assert( _t >= 0.f && _t <= 1.f );
	return _start + ( ( _end - _start ) * _t );
}

