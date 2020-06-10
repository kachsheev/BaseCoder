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

template<typename T, unsigned long long shiftValue, bool ...values>
struct ShiftSequence;

template<typename T, unsigned long long shiftValue, bool flag, bool ...flags>
struct ShiftSequence<T, shiftValue, flag, flags...>
{
public:
	static constexpr T value = (static_cast<T>(flag) << shiftValue)
			| ShiftSequence<T, shiftValue - 1, flags...>::value;
};

template<typename T, unsigned long long shiftValue>
struct ShiftSequence<T, shiftValue>
{
	static constexpr T value = 0;
};

template<typename T, bool ...flags>
struct MaskCreator
{
	static constexpr T value = detail::ShiftSequence<T, sizeof...(flags) - 1, flags...>::value;
};

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

	static_assert(!std::is_same_v<Type, void>, "Invalid bits");
};

template<unsigned long long countUppedBits>
struct SpetializedMaskCreator
{
	using Type = typename ChooseNumber<countUppedBits>::Type;
	static constexpr Type value = (static_cast<Type>(true) << (countUppedBits - 1))
			| SpetializedMaskCreator<countUppedBits - 1>::value;
};

template<>
struct SpetializedMaskCreator<0>
{
	static constexpr bool value = false;
};

} // namespace datail

template<unsigned long long countUppedBits>
using NumberType = typename detail::ChooseNumber<countUppedBits>::Type;

template<unsigned long long countUppedBits>
constexpr NumberType<countUppedBits> uppedMask =
		detail::SpetializedMaskCreator<countUppedBits>::value;

} // namespace base_coder

#endif // BASECODER_META_HPP
