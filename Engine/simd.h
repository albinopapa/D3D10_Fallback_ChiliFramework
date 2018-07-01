#pragma once

#include <cassert>
#include <intrin.h>
#include <type_traits>

class int128;
class uint128;
class float128;


template<class T> struct is_integral : std::false_type{};
template<class T> struct is_signed : std::false_type{};
template<class T> struct is_unsigned : std::false_type{};
template<class T> struct is_floating_point : std::false_type{};

template<int _count, class T> T _vectorcall shift( const T _val );
inline float128 _vectorcall operator<( const float128 _lhs, const float128 _rhs );
inline float128 _vectorcall operator>( const float128 _lhs, const float128 _rhs );
inline float128 _vectorcall operator<=( const float128 _lhs, const float128 _rhs );
inline float128 _vectorcall operator>=( const float128 _lhs, const float128 _rhs );
inline bool     _vectorcall IsBitSet( const float128 val, const int _bitpos );

class int128
{
public:
	int128() = default;
	explicit int128( __m128i _dqword )
		:
		value( _dqword )
	{}
	int128( int32_t _val )
		:
		value( _mm_set1_epi32( _val ) )
	{}
	int128( int32_t _val0, int32_t _val1, int32_t _val2, int32_t _val3 )
		:
		value( _mm_set_epi32( _val0, _val1, _val2, _val3 ) )
	{}
	int128( const int32_t* pVal )
	{
		assert( pVal != nullptr );
		const auto* mpVal = reinterpret_cast< const __m128i* >( pVal );
		value = ( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 ) ?
			_mm_load_si128( mpVal ) : _mm_loadu_si128( mpVal );
	}

	_vectorcall operator __m128i( )const
	{
		return value;
	}
	int128  _vectorcall operator+( const int128 _other )const
	{
		return int128( _mm_add_epi32( value, _other.value ) );
	}
	int128& _vectorcall operator+=( const int128 _other )
	{
		*this = *this + _other;
		return *this;
	}
	int128  _vectorcall operator-( const int128 _other )const
	{
		return int128( _mm_sub_epi32( value, _other.value ) );
	}
	int128& _vectorcall operator-=( const int128 _other )
	{
		*this = *this - _other;
		return *this;
	}
	int128  _vectorcall operator*( const int128 _other )const
	{
		__m128i src_up_lo = _mm_unpacklo_epi32( value, _mm_setzero_si128() );
		__m128i src_up_hi = _mm_unpackhi_epi32( value, _mm_setzero_si128() );

		__m128i dst_up_lo = _mm_unpacklo_epi32( _other.value, _mm_setzero_si128() );
		__m128i dst_up_hi = _mm_unpackhi_epi32( _other.value, _mm_setzero_si128() );

		__m128i res_lo = _mm_mul_epu32( src_up_lo, dst_up_lo );
		__m128i res_hi = _mm_mul_epu32( src_up_hi, dst_up_hi );

		return int128( _mm_packs_epi32( res_lo, res_hi ) );
	}
	int128& _vectorcall operator*=( const int128 _other )
	{
		*this = *this * _other;
		return *this;
	}
	int128  _vectorcall operator<<( const int _count )const
	{
		return int128( _mm_slli_epi32( value, _count ) );
	}
	int128& _vectorcall operator<<=( const int _count )
	{
		*this = *this << _count;
		return *this;
	}
	int128  _vectorcall operator>>( const int _count )const
	{
		return int128( _mm_srli_epi32( value, _count ) );
	}
	int128& _vectorcall operator>>=( const int _count )
	{
		*this = *this >> _count;
		return *this;
	}
	int128  _vectorcall operator&( const int128 _mask )const
	{
		return int128( _mm_and_si128( _mask.value, value ) );
	}
	int128& _vectorcall operator&=( const int128 _mask )
	{
		*this = *this & _mask;
		return *this;
	}
	int128  _vectorcall operator|( const int128 _mask )const
	{
		return int128( _mm_or_si128( _mask.value, value ) );
	}
	int128& _vectorcall operator|=( const int128 _mask )
	{
		*this = *this | _mask;
		return *this;
	}
	void _vectorcall Store( int* const pVal )const
	{
		assert( pVal != nullptr );
		if( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 )
		{
			_mm_store_si128( reinterpret_cast< __m128i* >( pVal ), value );
		}
		else
		{
			_mm_storeu_si128( reinterpret_cast< __m128i* >( pVal ), value );
		}
	}

private:
	template<int _count, class T> friend T _vectorcall shift( const T _val );
	
	
private:
	__m128i value;
};

class uint128
{
public:
	uint128() = default;
	explicit uint128( __m128i _dqword )
		:
		value( _dqword )
	{}
	uint128( uint32_t _val )
		:
		value( _mm_set1_epi32( _val ) )
	{}
	uint128( uint32_t _val0, uint32_t _val1, uint32_t _val2, uint32_t _val3 )
		:
		value( _mm_set_epi32( _val0, _val1, _val2, _val3 ) )
	{}
	uint128( const uint32_t* pVal )
	{
		assert( pVal != nullptr );
		const auto* mpVal = reinterpret_cast< const __m128i* >( pVal );
		value = ( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 ) ?
			_mm_load_si128( mpVal ) : _mm_loadu_si128( mpVal );
	}

	_vectorcall operator __m128i( )const
	{
		return value;
	}
	uint128  _vectorcall operator+( const uint128 _other )const
	{
		return uint128( _mm_add_epi32( value, _other.value ) );
	}
	uint128& _vectorcall operator+=( const uint128 _other )
	{
		*this = *this + _other;
		return *this;
	}
	uint128  _vectorcall operator-( const uint128 _other )const
	{
		return uint128( _mm_sub_epi32( value, _other.value ) );
	}
	uint128& _vectorcall operator-=( const uint128 _other )
	{
		*this = *this - _other;
		return *this;
	}
	uint128  _vectorcall operator*( const uint128 _other )const
	{
		__m128i src_up_lo = _mm_unpacklo_epi32( value, _mm_setzero_si128() );
		__m128i src_up_hi = _mm_unpackhi_epi32( value, _mm_setzero_si128() );

		__m128i dst_up_lo = _mm_unpacklo_epi32( _other.value, _mm_setzero_si128() );
		__m128i dst_up_hi = _mm_unpackhi_epi32( _other.value, _mm_setzero_si128() );

		__m128i res_lo = _mm_mul_epu32( src_up_lo, dst_up_lo );
		__m128i res_hi = _mm_mul_epu32( src_up_hi, dst_up_hi );

		return uint128( _mm_packus_epi32( res_lo, res_hi ) );
	}
	uint128& _vectorcall operator*=( const uint128 _other )
	{
		*this = *this * _other;
		return *this;
	}
	uint128  _vectorcall operator<<( const int32_t _count )const
	{
		return uint128( _mm_slli_epi32( value, _count ) );
	}
	uint128& _vectorcall operator<<=( const int32_t _count )
	{
		*this = *this << _count;
		return *this;
	}
	uint128  _vectorcall operator>>( const int32_t _count )const
	{
		return uint128( _mm_srli_epi32( value, _count ) );
	}
	uint128& _vectorcall operator>>=( const int32_t _count )
	{
		*this = *this >> _count;
		return *this;
	}
	uint128  _vectorcall operator&( const uint128 _mask )const
	{
		return uint128( _mm_and_si128( _mask.value, value ) );
	}
	uint128& _vectorcall operator&=( const uint128 _mask )
	{
		*this = *this & _mask;
		return *this;
	}
	uint128  _vectorcall operator|( const uint128 _mask )const
	{
		return uint128( _mm_or_si128( _mask.value, value ) );
	}
	uint128& _vectorcall operator|=( const uint128 _mask )
	{
		*this = *this | _mask;
		return *this;
	}

	void _vectorcall Store( uint32_t* const pVal )const
	{
		assert( pVal != nullptr );
		if( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 )
		{
			_mm_store_si128( reinterpret_cast< __m128i* >( pVal ), value );
		}
		else
		{
			_mm_storeu_si128( reinterpret_cast< __m128i* >( pVal ), value );
		}
	}


private:
	template<int _count, class T> friend T _vectorcall shift( const T _val );
	

	
private:
	__m128i value;
};

class float128
{
public:
	float128() = default;
	explicit float128( __m128 _dqword )
		:
		value( _dqword )
	{}
	float128( float _val )
		:
		value( _mm_set_ps1( _val ) )
	{}
	float128( float _val0, float _val1, float _val2, float _val3 )
		:
		value( _mm_set_ps( _val0, _val1, _val2, _val3 ) )
	{}
	float128( const float* pVal )
	{
		assert( pVal != nullptr );
		value = ( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 ) ?
			_mm_load_ps( pVal ) : _mm_loadu_ps( pVal );
	}

	explicit _vectorcall operator int128()const
	{
		return int128( _mm_cvtps_epi32( value ) );
	}
	_vectorcall operator __m128( )const
	{
		return value;
	}
	float128  _vectorcall operator+(   const float128 _other )const
	{
		return float128( _mm_add_ps( value, _other.value ) );
	}
	float128& _vectorcall operator+=(  const float128 _other )
	{
		*this = *this + _other;
		return *this;
	}
	float128  _vectorcall operator-(   const float128 _other )const
	{
		return float128( _mm_sub_ps( value, _other.value ) );
	}
	float128& _vectorcall operator-=(  const float128 _other )
	{
		*this = *this - _other;
		return *this;
	}
	float128  _vectorcall operator*(   const float128 _other )const
	{
		return float128( _mm_mul_ps( value, _other.value ) );
	}
	float128& _vectorcall operator*=(  const float128 _other )
	{
		*this = *this * _other;
		return *this;
	}
	float128  _vectorcall operator/(   const float128 _other )const
	{
		return float128( _mm_div_ps( value, _other.value ) );
	}
	float128& _vectorcall operator/=(  const float128 _other )
	{
		*this = *this / _other;
		return *this;
	}
	float128  _vectorcall operator&(   const float128 _mask )const
	{
		return float128( _mm_and_ps( _mask.value, value ) );
	}
	float128& _vectorcall operator&=(  const float128 _mask )
	{
		*this = *this & _mask;
		return *this;
	}
	float128  _vectorcall operator|(   const float128 _mask )const
	{
		return float128( _mm_or_ps( _mask.value, value ) );
	}
	float128& _vectorcall operator|=(  const float128 _mask )
	{
		*this = *this | _mask;
		return *this;
	}
		
	bool AllTrue()const
	{
		mask = _mm_movemask_ps( value ) == 15;
	}
	bool AllFalse()const
	{
		return _mm_movemask_ps( value ) == 0;
	}

	void _vectorcall Store( float* const pVal )const
	{
		assert( pVal != nullptr );
		if( reinterpret_cast< uintptr_t >( pVal ) % 16 == 0 )
		{
			_mm_store_ps( pVal, value );
		}
		else
		{
			_mm_storeu_ps( pVal, value );
		}
	}

private:
	template<size_t A, size_t B, size_t C, size_t D, class T>
	friend std::enable_if_t<is_floating_point<T>::value, T> _vectorcall shuffle( const T _lhs, const T _rhs );
	friend float128 _vectorcall operator<( const float128 _lhs, const float128 _rhs );
	friend float128 _vectorcall operator>( const float128 _lhs, const float128 _rhs );
	friend float128 _vectorcall operator<=( const float128 _lhs, const float128 _rhs );
	friend float128 _vectorcall operator>=( const float128 _lhs, const float128 _rhs );
	friend bool		_vectorcall IsBitSet( const float128 val, const int _bitpos );

	
private:
	__m128 value;
	mutable int mask = 0;
};

template<class T> class simd_traits{};
template<> struct is_integral<int128> : std::true_type{};
template<> struct is_integral<uint128> : std::true_type{};
template<> struct is_signed<int128> : std::true_type{};
template<> struct is_unsigned<uint128> : std::true_type{};
template<> struct is_floating_point<float128> : std::true_type{};

struct sse_type
{
	static constexpr auto bit_width = sizeof( __m128i ) * 8;
	static constexpr auto byte_width = sizeof( __m128i );
	static constexpr auto num_elements = sizeof( __m128i ) / 4;
};
template<> struct simd_traits<int128> : sse_type
{
	using store_type = int*;
	inline static const int128 zero = int128( _mm_setzero_si128() );
	inline static const int128 one = int128( _mm_set1_epi32( 1 ) );
};
template<> struct simd_traits<uint128> : sse_type
{
	using store_type = unsigned int*;
	inline static const uint128 zero = uint128( _mm_setzero_si128() );
	inline static const uint128 one = uint128( _mm_set1_epi32( 1 ) );
};
template<> struct simd_traits<float128> : sse_type
{
	using store_type = float*;
	inline static const float128 zero = float128( _mm_setzero_ps() );
	inline static const float128 one = float128{ _mm_set_ps1( 1.f ) };
	inline static const float128 half = float128{ _mm_set_ps1( .5f ) };
};

template<size_t A, size_t B, size_t C, size_t D>
constexpr int shuffle_mask = ( D << 6 ) | ( C << 4 ) | ( B << 2 ) | A;

template<class T>
constexpr size_t num_elements_v = simd_traits<T>::num_elements;


// Shifts all bytes in register by _count bytes to the left if positive
// Shifts all bytes in register by _count bytes to the right if negative
// Shifting left here means shifting bytes to higher addresses which may appear to be shifting to the right
// Only works on integral types
template<int _count, class T> T _vectorcall shift( const T _val )
{
	static_assert( _count > -4 && _count < 4, "-3 to 3 is the valid range, 0 returns _val." );
	constexpr int count = _count * ( _count < 0 ? -4 : 4 );

	if constexpr( _count > 0 )
	{
		return T( _mm_slli_si128( _val.value, count ) );
	}
	else if constexpr( _count < 0 )
	{
		return T( _mm_srli_si128( _val.value, count ) );
	}
	else
	{
		return T( _val );
	}
}


template<size_t A, size_t B, size_t C, size_t D, class T>
std::enable_if_t<is_floating_point<T>::value,T> _vectorcall shuffle( const T _lhs, const T _rhs )
{
	return T( _mm_shuffle_ps( _lhs.value, _rhs.value, shuffle_mask<A, B, C, D> ) );
}

float128 _vectorcall operator<( const float128 _lhs, const float128 _rhs )
{
	return float128( _mm_cmplt_ps( _lhs.value, _rhs.value ) );
}
float128 _vectorcall operator>( const float128 _lhs, const float128 _rhs )
{
	return float128( _mm_cmpgt_ps( _lhs.value, _rhs.value ) );
}
float128 _vectorcall operator<=( const float128 _lhs, const float128 _rhs )
{
	return float128( _mm_cmple_ps( _lhs.value, _rhs.value ) );
}
float128 _vectorcall operator>=( const float128 _lhs, const float128 _rhs )
{
	return float128( _mm_cmpge_ps( _lhs.value, _rhs.value ) );
}

bool _vectorcall IsBitSet( const float128 val, const int _bitpos )
{
	const auto bitpos = _bitpos & 0xf;
	const auto mask = _mm_movemask_ps( val.value );
	return ( mask & ( 1 << bitpos ) ) != 0;
}