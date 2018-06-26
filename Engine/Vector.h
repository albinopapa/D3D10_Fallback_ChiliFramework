#pragma once

#include "template_math_ops.h"
#include <type_traits>

template<size_t ElemCount, typename ElemType>
class Vector{};

template<typename T>
class Vector<2, T>
{
public:
	static constexpr size_t elem_count = 2;
public:
	Vector() = default;
	constexpr Vector( T _x, T _y ) : x( _x ), y( _y ){}
	Vector& operator+=( const Vector& _other )
	{
		x += _other.x;
		y += _other.y;
		return *this;
	}
	Vector& operator-=( const Vector& _other )
	{
		x -= _other.x;
		y -= _other.y;
		return *this;
	}
	Vector& operator*=(const T& _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		return *this;
	}
	Vector& operator/=(const T& _scalar )
	{
		if constexpr( std::is_floating_point_v<T> )
		{
			const auto invScalar = T( 1. ) / _scalar;
			return *this *= invScalar;
		}
		x /= _scalar;
		y /= _scalar;
		return *this;
	}
public:
	T x, y;
};

template<typename T>
class Vector<3, T>
{
public:
	static constexpr size_t elem_count = 3;
public:
	Vector() = default;
	constexpr Vector( T _x, T _y, T _z ) : x( _x ), y( _y ), z( _z ){}

	Vector& operator+=( const Vector& _other )
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		return *this;
	}
	Vector& operator-=( const Vector& _other )
	{
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		return *this;
	}
	Vector& operator*=( const T& _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		return *this;
	}
	Vector& operator/=( const T& _scalar )
	{
		if constexpr( std::is_floating_point_v<T> )
		{
			const auto invScalar = T( 1. ) / _scalar;
			return *this *= invScalar;
		}

		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		return *this;
	}
public:
	T x, y, z;
};


template<class T> using Vec2 = Vector<2, T>;
template<class T> using Vec3 = Vector<3, T>;
using Vec2f = Vector<2, float>;
using Vec3f = Vector<3, float>;