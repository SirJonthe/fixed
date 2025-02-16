/// @file fixed.h
/// @brief Contains an implementation of a fixed-point data type with customizable width and precision.
/// @author github.com/SirJonthe
/// @date 2025
/// @copyright Public domain.
/// @license CC0 1.0

#ifndef CC0_FIXED_H_INCLUDED__
#define CC0_FIXED_H_INCLUDED__

#include <cstdint>

namespace cc0
{
	/// @brief For internal use only.
	namespace fixed_internal
	{
		/// @brief Provides some type information for various integer types of specified bit sizes.
		/// @tparam bits The number of bits to get integer information about.
		template < uint32_t bits > struct intinfo {};

		/// @brief Provides some type information for various integer types of 8-bit width.
		template <>
		struct intinfo<8>
		{
			typedef int8_t      int_t;  // An 8-bit signed type.
			typedef uint8_t     uint_t; // An 8-bit unsigned type.
			typedef intinfo<8>  prev;   // The previous, smaller type info object (there is none, so we alias the same object).
			typedef intinfo<16> next;   // The next, larger type info object for when casting to the next larger type is needed.
		};

		/// @brief Provides some type information for various integer types of 16-bit width.
		template <>
		struct intinfo<16>
		{
			typedef int16_t     int_t;  // An 16-bit signed type.
			typedef uint16_t    uint_t; // An 16-bit unsigned type.
			typedef intinfo<8>  prev;   // The previous, smaller type info object for when casting to the previous smaller type is needed.
			typedef intinfo<32> next;   // The next, larger type info object for when casting to the next larger type is needed.
		};

		/// @brief Provides some type information for various integer types of 32-bit width.
		template <>
		struct intinfo<32>
		{
			typedef int32_t     int_t;  // An 32-bit signed type.
			typedef uint32_t    uint_t; // An 32-bit unsigned type.
			typedef intinfo<16> prev;   // The previous, smaller type info object for when casting to the previous smaller type is needed.
			typedef intinfo<64> next;   // The next, larger type info object for when casting to the next larger type is needed.
		};

		/// @brief Provides some type information for various integer types of 64-bit width.
		template <>
		struct intinfo<64>
		{
			typedef int64_t     int_t;  // An 64-bit signed type.
			typedef uint64_t    uint_t; // An 64-bit unsigned type.
			typedef intinfo<32> prev;   // The previous, smaller type info object for when casting to the previous smaller type is needed.
			typedef intinfo<64> next;   // The next, larger type info object (there is none, so we alias the same object).
		};

		/// @brief The log10.
		/// @param x The number.
		/// @return The result.
		inline uint32_t log10(uint64_t x)
		{
			uint32_t n;
			for (n = 0; x > 0; ++n, x /= 10) {}
			return n - 1;
		}

		/// @brief Exponentiates a number by another number.
		/// @param x The base.
		/// @param n The exponent.
		/// @return The result.
		inline uint64_t exp(uint64_t x, int32_t n)
		{
			uint64_t X = 1;
			for (; n > 0; --n) {
				X *= x;
			}
			return X;
		}
	}

	/// @brief A real number with a fixed number of bits dedicated for decimals.
	/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
	/// @tparam precision The number of bits dedicated to decimals.
	/// @note 64 bits for base type might very easily lead to overflow when multiplying.
	template < uint32_t bits, uint32_t precision >
	class fixed
	{
	private:
		typedef typename cc0::fixed_internal::intinfo<bits>::int_t int_t;
		typedef typename cc0::fixed_internal::intinfo<bits>::uint_t uint_t;

	public:
		int_t value_bits; // The binary representation of the fixed-point number.

		/// @brief The default constructor. Does nothing, and does not initialize the instance.
		fixed( void ) = default;
		
		/// @brief The copy constructor.
		/// @param NA The instance to copy.
		fixed(const fixed&) = default;
		
		/// @brief The copy operator.
		/// @param NA The instance to copy.
		/// @return The result.
		fixed &operator=(const fixed&) = default;

		/// @brief A conversion constructor that converts an integer into a fixed-point number by upscaling it.
		/// @param n The number to upscale into a fixed-point number.
		fixed(int_t n) : value_bits(n << precision) {}

		/// @brief Converts two integers into a fixed-point number.
		/// @param i The integer part of the number.
		/// @param d The fractional part of the number in base 10.
		/// @note The fractional parameter is scaled properly, meaning 9 is the same as 90, 900, 9000, etc.
		fixed(int_t i, uint_t d);

		/// @brief A conversion operator converting the fixed-point number into an integer by downscaling it.
		operator int_t( void ) const { return value_bits >> precision; }

		/// @brief Addition.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator+=(fixed r) { value_bits += r.value_bits; return *this; }

		/// @brief Subtraction.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator-=(fixed r) { value_bits -= r.value_bits; return *this; }
		
		/// @brief Multiplication.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator*=(fixed r) {
			typename fixed_internal::intinfo<bits>::next::int_t n = typename fixed_internal::intinfo<bits>::next::int_t(value_bits) * r.value_bits;
			value_bits = (n >> precision);
			return *this;
		}

		/// @brief Division.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator/=(fixed r) {
			value_bits = (typename fixed_internal::intinfo<bits>::next::int_t(value_bits) << precision) / r.value_bits;
			return *this;
		}

		/// @brief Addition.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator+=(int_t r) { value_bits += (r.value_bits << precision); return *this; }

		/// @brief Subtraction.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator-=(int_t r) { value_bits -= (r.value_bits << precision); return *this; }
		
		/// @brief Multiplication.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator*=(int_t r) { value_bits *= r; return *this; }

		/// @brief Division.
		/// @param r The right-hand side operator.
		/// @return The result.
		fixed &operator/=(int_t r) { value_bits /= r; return *this; }
	};
}

template < uint32_t bits, uint32_t precision >
cc0::fixed<bits,precision>::fixed(cc0::fixed<bits,precision>::int_t i, cc0::fixed<bits,precision>::uint_t d) : value_bits(i << precision)
{
	static constexpr typename cc0::fixed_internal::intinfo<bits>::uint_t MAX_FRAC        = (typename cc0::fixed_internal::intinfo<bits>::uint_t(1) << precision) - 1;
	static const uint32_t                                                MAX_FRAC_LOG10  = cc0::fixed_internal::log10(MAX_FRAC) - 1;
	static const uint32_t                                                MAX_FRAC_BASE10 = cc0::fixed_internal::exp(10, MAX_FRAC_LOG10) - 1;
	static const cc0::fixed<bits,bits-precision-1>                       SCALE           = cc0::fixed<bits,bits-precision-1>(MAX_FRAC) / MAX_FRAC_BASE10;
	
	if (d > 0) {
		const uint32_t log10 = cc0::fixed_internal::log10(d);
		if (log10 > MAX_FRAC_LOG10) {
			d /= cc0::fixed_internal::exp(10, log10 - MAX_FRAC_LOG10);
		} else {
			d *= cc0::fixed_internal::exp(10, MAX_FRAC_LOG10 - log10);
		}
		value_bits += typename cc0::fixed_internal::intinfo<bits>::int_t(d * SCALE);
	}
}

/// @brief Addition.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator+(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l += r; }

/// @brief Subtraction.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator-(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l -= r; }

/// @brief Multiplication.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator*(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l *= r; }

/// @brief Division.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator/(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l /= r; }

/// @brief Addition.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator+(cc0::fixed<bits,precision> l, typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t r) { return l += r; }

/// @brief Subtraction.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator-(cc0::fixed<bits,precision> l, typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t r) { return l -= r; }

/// @brief Multiplication.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator*(cc0::fixed<bits,precision> l, typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t r) { return l *= r; }

/// @brief Division.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator/(cc0::fixed<bits,precision> l, typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t r) { return l /= r; }

/// @brief Addition.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator+(typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return r += l; }

/// @brief Subtraction.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator-(typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return cc0::fixed<bits,precision>(l) -= r; }

/// @brief Multiplication.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator*(typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return r *= l; }

/// @brief Division.
/// @tparam bits The total number of bits for the base data type. Supported sizes are 8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline cc0::fixed<bits,precision> operator/(typename cc0::fixed<bits,precision>::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return cc0::fixed<bits,precision>(l) /= r; }

/// @brief Equality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator==(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits == r.value_bits; }

/// @brief Inequality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator!=(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits != r.value_bits; }

/// @brief Less than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits < r.value_bits; }

/// @brief Greater than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits > r.value_bits; }

/// @brief Less or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<=(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits <= r.value_bits; }

/// @brief Greater or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>=(cc0::fixed<bits,precision> l, cc0::fixed<bits,precision> r) { return l.value_bits >= r.value_bits; }

/// @brief Equality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator==(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) == r.value_bits; }

/// @brief Inequality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator!=(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) != r.value_bits; }

/// @brief Less than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) < r.value_bits; }

/// @brief Greater than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) > r.value_bits; }

/// @brief Less or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<=(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) <= r.value_bits; }

/// @brief Greater or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>=(typename cc0::fixed_internal::intinfo<bits>::int_t l, cc0::fixed<bits,precision> r) { return (typename cc0::fixed_internal::intinfo<bits>::next::int_t(l) << precision) >= r.value_bits; }

/// @brief Equality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator==(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits == (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

/// @brief Inequality.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator!=(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits != (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

/// @brief Less than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits < (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

/// @brief Greater than.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits > (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

/// @brief Less or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator<=(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits <= (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

/// @brief Greater or equal.
/// @tparam bits The total number of bits for the base data type. Supported sizes are8, 16, 32, and 64.
/// @tparam precision The number of bits dedicated to decimals.
/// @param l The left-hand side operand.
/// @param r The right-hand side operand.
/// @return The result.
template < uint32_t bits, uint32_t precision > inline bool operator>=(cc0::fixed<bits,precision> l, typename cc0::fixed_internal::intinfo<bits>::int_t r) { return l.value_bits >= (typename cc0::fixed_internal::intinfo<bits>::next::int_t(r) << precision); }

#endif
