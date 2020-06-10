#ifndef BASECODER_BASECODER_TRAITS_HPP
#define BASECODER_BASECODER_TRAITS_HPP

#include <climits>
#include <cstdint>

#if !defined(CHAR_BIT)
#error Need defined 'CHAR_BIT' macro
#endif

static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

namespace base_coder
{

///
/// \brief The Type enum
///
enum class Type
{
	Base64, Base32, Base16
};

///
/// \brief The Subtype enum
///
enum class Subtype
{
	Common, Hex
};

///
/// \brief The AlphabetTraits struct
///
template<Type TYPE = Type::Base64, Subtype SUBTYPE = Subtype::Common>
struct AlphabetTraits
{
	using AlphabetType = char;
	static constexpr AlphabetType alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno"
			"pqrstuvwxyz0123456789+/";
	static constexpr auto alphabetSize = (sizeof(alphabet) / sizeof(AlphabetType)) - 1;
	static constexpr AlphabetType pad = '=';
};

///
/// \brief The AlphabetTraits<Type::BASE64, Subtype::HEX> struct
///
template<>
struct AlphabetTraits<Type::Base64, Subtype::Hex>
{
	using AlphabetType = char;
	static constexpr AlphabetType alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmno"
			"pqrstuvwxyz0123456789-_";
	static constexpr auto alphabetSize = (sizeof(alphabet) / sizeof(AlphabetType)) - 1;
	static constexpr AlphabetType pad = '=';
};

///
/// \brief The AlphabetTraits<Type::BASE32, Subtype::BASE> struct
///
template<>
struct AlphabetTraits<Type::Base32, Subtype::Common>
{
	using AlphabetType = char;
	static constexpr AlphabetType alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
	static constexpr auto alphabetSize = (sizeof(alphabet) / sizeof(AlphabetType)) - 1;
	static constexpr AlphabetType pad = '=';
};

///
/// \brief The AlphabetTraits<Type::BASE32, Subtype::HEX> struct
///
template<>
struct AlphabetTraits<Type::Base32, Subtype::Hex>
{
	using AlphabetType = char;
	static constexpr AlphabetType alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	static constexpr auto alphabetSize = (sizeof(alphabet) / sizeof(AlphabetType)) - 1;
	static constexpr AlphabetType pad = '=';
};

///
/// \brief The AlphabetTraits<Type::BASE16, Subtype::BASE> struct
///
template<>
struct AlphabetTraits<Type::Base16, Subtype::Common>
{
	using AlphabetType = char;
	static constexpr AlphabetType alphabet[] = "0123456789ABCDEF";
	static constexpr auto alphabetSize = (sizeof(alphabet) / sizeof(AlphabetType)) - 1;
	static constexpr AlphabetType pad = '=';
};

///
/// \brief The Traits struct
///
template<Type TYPE, Subtype SUBTYPE>
struct Traits : public AlphabetTraits<TYPE, SUBTYPE>
{
	using AlphabetTraits<TYPE, SUBTYPE>::AlphabetType;
	using AlphabetTraits<TYPE, SUBTYPE>::alphabet;
	using AlphabetTraits<TYPE, SUBTYPE>::pad;

	static_assert(!(TYPE == Type::Base16 && SUBTYPE == Subtype::Hex), "Imposible format");

	static constexpr auto type = TYPE;
	static constexpr auto subtype = SUBTYPE;

	static constexpr std::size_t indexBitSize = (TYPE == Type::Base64) ? 6
			: (TYPE == Type::Base32) ? 5 : 4;
	static constexpr std::size_t indexBufferSize = (TYPE == Type::Base64) ? 4
			: (TYPE == Type::Base32) ? 8 : 2;

	static constexpr std::size_t indexBufferSizeInBits = indexBitSize * indexBufferSize;

	static constexpr std::size_t inputBufferSize = indexBufferSizeInBits / CHAR_BIT;
};

using Base64Traits = Traits<Type::Base64, Subtype::Common>;
using Base64HexTraits = Traits<Type::Base64, Subtype::Hex>;
using Base32Traits = Traits<Type::Base32, Subtype::Common>;
using Base32HexTraits = Traits<Type::Base32, Subtype::Hex>;
using Base16Traits = Traits<Type::Base16, Subtype::Common>;

} // namespace base_coder

#endif // BASECODER_BASECODER_TRAITS_HPP
