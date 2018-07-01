#pragma once

#include "Vector.h"

template<class T>
class Rect
{
public:
	Rect() = default;
	constexpr Rect( T _left, T _top, T _right, T _bottom )
		:
		left( _left ), top( _top ), right( _right ), bottom( _bottom )
	{}
	constexpr Rect( Vec2<T> _left_top, Vec2<T> _right_bottom )
		:
		Rect( _left_top.x, _left_top.y, _right_bottom.x, _right_bottom.y )
	{}
	template<class U> constexpr operator Rect<U>()const
	{
		return { U( left ),U( top ),U( right ),U( bottom ) };
	}
	Rect& operator+=( const Vec2<T>& _offset )
	{
		left	+= _offset.x;
		top		+= _offset.y;
		right	+= _offset.x;
		bottom	+= _offset.y;

		return *this;
	}
	Rect& operator*=( const float _scale )
	{
		left	*= _scale;
		top		*= _scale;
		right	*= _scale;
		bottom	*= _scale;

		return *this;
	}
public:
	T left, top, right, bottom;
};

template<class T> Rect<T> operator+( const Rect<T>& _rect, const Vec2<T>& _offset );
template<class T> T RectWidth( const Rect<T>& _rect );
template<class T> T RectHeight( const Rect<T>& _rect );
template<class T> Vec2<T> RectCenter( const Rect<T>& _lhs );
template<class T> bool RectContains( const Rect<T>& _lhs, const Rect<T>& _rhs );
template<class T> bool RectContains( const Rect<T>& _lhs, const Vec2<T>& _rhs );
template<class T> bool RectOverlaps( const Rect<T>& _lhs, const Rect<T>& _rhs );

template<class T> Rect<T> operator+( const Rect<T>& _rect, const Vec2<T>& _offset )
{
	return Rect<T>( _rect ) += _offset;
}
template<class T> Vec2<T> RectCenter( const Rect<T>& _lhs )
{
	return Lerp( Vec2f{ _lhs.left,_lhs.top }, Vec2f{ _lhs.right, _lhs.bottom }, .5f );
}
template<class T> T RectWidth( const Rect<T>& _rect )
{
	return std::abs( _rect.right - _rect.left );
}
template<class T> T RectHeight( const Rect<T>& _rect )
{
	return std::abs( _rect.bottom - _rect.top );
}

template<class T> bool RectContains( const Rect<T>& _lhs, const Rect<T>& _rhs )
{
	return
		_lhs.left < _rhs.left && _lhs.right > _rhs.right &&
		_lhs.top < _rhs.top && _lhs.bottom > _rhs.bottom;
}

template<class T> bool RectContains( const Rect<T>& _lhs, const Vec2<T>& _rhs )
{
	return
		_lhs.left < _rhs.x && _lhs.right > _rhs.x &&
		_lhs.top < _rhs.y && _lhs.bottom > _rhs.y;
}

template<class T> bool RectOverlaps( const Rect<T>& _lhs, const Rect<T>& _rhs )
{
	return
		_lhs.right >= _rhs.left && _lhs.left <= _rhs.right &&
		_lhs.top >= _rhs.bottom && _lhs.bottom <= _rhs.top;
}

using RectF = Rect<float>;