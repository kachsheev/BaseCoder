#ifndef BASECODER_META_HPP
#define BASECODER_META_HPP

#include <climits>
#include <type_traits>

#if !defined(CHAR_BIT)
#error Need defined 'CHAR_BIT' macro
#endif

static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

namespace base_coder
{

namespace detail
{

template<unsigned long long bits>
struct ChooseNumber
{
	static constexpr decltype(bits) UcharSize = CHAR_BIT * sizeof(unsigned char);
	static constexpr decltype(bits) UshortSize = CHAR_BIT * sizeof(unsigned short);
	static constexpr decltype(bits) UintSize = CHAR_BIT * sizeof(unsigned int);
	static constexpr decltype(bits) UlongSize = CHAR_BIT * sizeof(unsigned long);
	static constexpr decltype(bits) UlonglongSize = CHAR_BIT * sizeof(unsigned long long);

	using Type = std::conditional_t<
		bits <= UcharSize
		, unsigned char
		, std::conditional_t<
			bits <= UshortSize
			, unsigned short
			, std::conditional_t<
				bits <= UintSize
				, unsigned int
				, std::conditional_t<
					bits <= UlongSize
					, unsigned long
					, std::conditional_t<
						bits <= UlongSize
						, unsigned long long
						, void
					>
				>
			>
		>
	>;

	static_assert(!std::is_same_v<Type, void>, "Invalid input count bits");
};

template<unsigned long long countUppedBits>
struct MaskCreator
{
	using Type = typename ChooseNumber<countUppedBits>::Type;
	static constexpr Type value = (static_cast<Type>(true) << (countUppedBits - 1))
			| MaskCreator<countUppedBits - 1>::value;
};

template<>
struct MaskCreator<0>
{
	static constexpr bool value = false;
};

} // namespace datail

template<unsigned long long countUppedBits>
using NumberType = typename detail::ChooseNumber<countUppedBits>::Type;

template<unsigned long long countUppedBits>
constexpr NumberType<countUppedBits> uppedMask =
		detail::MaskCreator<countUppedBits>::value;

} // namespace base_coder

#endif // BASECODER_META_HPP
