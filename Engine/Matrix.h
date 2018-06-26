#pragma once

#include "Vector.h"

template<size_t Rows, size_t Cols, class T>
class Matrix
{
public:
	Matrix() = default;
	static Matrix Identity()
	{
		Matrix mat;
		for( int row = 0; row < Rows; ++row )
		{
			for( int col = 0; col < Cols; ++col )
			{
				mat( row, col ) = T( row == col ? 1 : 0 );
			}
		}

		return mat;
	}
	static Matrix Translation( const Vector<Cols, T>& _offset )
	{
		Matrix mat = Identity();
		const T* pOffset = reinterpret_cast<const T* >( &_offset );
		for( int i = 0; i < Cols; ++i, ++pOffset )
		{
			mat( Rows - 1, i ) = *pOffset;
		}

		return mat;
	}
	static Matrix Scaling( const Vector<Rows <= Cols ? Rows : Cols, T>& _scale )
	{
		Matrix mat = Identity();
		const T* pScale = reinterpret_cast<const T* >( &_scale );

		for( int i = 0; i < ( Rows <= Cols ? Rows : Cols ); ++i, ++pScale )
		{
			mat( i, i ) = *pScale;
		}

		return mat;
	}
	static Matrix Rotation( const float _angle )
	{
		Matrix mat = Identity();
		const auto sine = std::sin( _angle );
		const auto cosine = std::cos( _angle );
		
		mat( 0, 0 ) = cosine; mat( 0, 1 ) = -sine;
		mat( 1, 0 ) = sine;   mat( 1, 1 ) = cosine;

		return mat;
	}
	T operator()( const int _row, const int _col )const
	{
		return m[ _row ][ _col ];
	}
	T& operator()( const int _row, const int _col )
	{
		return m[ _row ][ _col ];
	}
	const Vector<Cols, T>& Row( const int Row )const
	{
		return rows[ Row ];
	}
	Vector<Cols, T>& Row( const int Row )
	{
		return rows[ Row ];
	}
	Vector<Rows, T> Column( const int Col )const
	{
		auto& self = *this;
		Vector<Rows, T> v;
		T* pv = reinterpret_cast< T* >( &v );
		for( int row = 0; row < Rows; ++row, ++pv )
		{
			*pv = self( row, Col );
		}

		return v;
	}
	Matrix<Cols, Rows, T> Transpose()const
	{
		auto& self = *this;
		Matrix<Cols, Rows, T> mat;
		for( int n = 0; n < Cols; ++n )
		{
			mat.Row( n ) = self.Column( n );
		}

		return mat;
	}
private:
	union
	{
		T m[ Rows ][ Cols ];
		Vector<Cols, T> rows[ Rows ];
	};	
};

template<class T> Matrix<3, 2, T> operator*( const Matrix<3, 2, T>& _lhs, const Matrix<3, 2, T>& _rhs )
{
	Matrix<3, 2, T> mat;
	Matrix<2, 3, T> temp = _rhs.Transpose();

	for( auto j = 0; j < 3; ++j )
	{
		const auto left_vec = Vec3<T>( _lhs( j, 0 ), _lhs( j, 1 ), T( j < 2 ? 0 : 1 ) );
		for( auto i = 0; i < 2; ++i )
		{
			mat( j, i ) = DotProduct( left_vec, temp.Row( i ) );
		}
	}

	return mat;
}

template<class T> Vector<2, T> operator*( const Matrix<3, 2, T>&_mat, const Vector<2, T>& _vec )
{
	Vector<2, T> result;
	result.x = ( _vec.x * _mat( 0, 0 ) ) + ( _vec.y * _mat( 1, 0 ) ) + _mat( 2, 0 );
	result.y = ( _vec.x * _mat( 0, 1 ) ) + ( _vec.y * _mat( 1, 1 ) ) + _mat( 2, 1 );
	return result;
}