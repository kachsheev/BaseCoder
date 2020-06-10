#ifndef BASECODER_HPP
#define BASECODER_HPP

#include <BaseCoder/Traits.hpp>
#include <BaseCoder/View.hpp>
#include <BaseCoder/Meta.hpp>

#include <functional>
#include <utility>

namespace base_coder
{

///
/// \brief The BaseCoder class
/// \tparam Trait
///
template<typename Trait>
class BaseCoder
{
public:
	using AlphabetType = typename Trait::AlphabetType;

	static constexpr auto type = Trait::type;
	static constexpr auto subtype = Trait::subtype;
	static constexpr auto alphabet = Trait::alphabet;
	static constexpr auto alphabetSize = Trait::alphabetSize;
	static constexpr auto pad = Trait::pad;
	static constexpr auto indexBitSize = Trait::indexBitSize;
	static constexpr auto indexBufferSizeInBits = Trait::indexBufferSizeInBits;

	///
	/// \brief encodeSize
	/// \tparam InputIterator
	/// \param inputView
	/// \return
	///
	template<typename InputIterator>
	size_t encodeSize(View<InputIterator> inputView) const;

	///
	/// \brief encodeSize
	/// \tparam Container
	/// \param container
	/// \return
	///
	template<typename Container>
	size_t encodeSize(const Container &container) const;

	///
	/// \brief decodeSize
	/// \tparam InputIterator
	/// \param inputView
	/// \return
	///
	template<typename InputIterator>
	size_t decodeSize(View<InputIterator> inputView) const;

	///
	/// \brief decodeSize
	/// \tparam Container
	/// \param container
	/// \return
	///
	template<typename Container>
	size_t decodeSize(const Container &inputView) const;

	///
	/// \brief encode
	/// \tparam InputIterator
	/// \tparam OutputIterator
	/// \param inputView
	/// \param outputIterator
	/// \return
	///
	template<typename InputIterator, typename OutputIterator>
	void encode(View<InputIterator> inputView, OutputIterator outputIterator) const;

	///
	/// \brief encode
	/// \tparam Container
	/// \tparam OutputIterator
	/// \param container
	/// \param outputIterator
	/// \return
	///
	template<typename Container, typename OutputIterator>
	void encode(const Container &container, OutputIterator outputIterator) const;

	///
	/// \brief decode
	/// \tparam InputIterator
	/// \tparam OutputIterator
	/// \param inputView
	/// \param outputIterator
	///
	template<typename InputIterator, typename OutputIterator>
	void decode(View<InputIterator> inputView, OutputIterator outputIterator) const;

	///
	/// \brief decode
	/// \tparam Container
	/// \tparam OutputIterator
	/// \param container
	/// \param outputIterator
	///
	template<typename Container, typename OutputIterator>
	void decode(const Container &container, OutputIterator outputIterator) const;

protected:
	using Buffer = NumberType<indexBufferSizeInBits>;

	using EncodeInput = typename Trait::EncodeInput;
	using EncodeOutput = typename Trait::EncodeOutput;
	using DecodeInput = typename Trait::DecodeInput;
	using DecodeOutput = typename Trait::DecodeOutput;

	///
	/// \brief coreEncode
	/// \param data
	/// \return
	///
	EncodeOutput coreEncode(EncodeInput data) const;

	///
	/// \brief coreDecode
	/// \param data
	/// \return
	///
	DecodeOutput coreDecode(DecodeInput data) const;

private:
	template<typename T, typename Callable>
	class FakeIterator
	{
	public:
		FakeIterator(Callable &&callable) : call{ std::move(callable) }
		{}

		T &operator*()
		{
			call();
			return t;
		}

		auto &operator++()
		{
			return *this;
		}
	private:
		T t{};
		Callable call;
	};
	template<typename T, typename Callable>
	static auto makeFakeIterator(T, Callable &&callable)
	{
		return FakeIterator<T, Callable>(std::move(callable));
	}

	template<typename Iterator>
	constexpr void checkIteratorType() const
	{
		using IteratorReturnType = std::remove_cv_t< std::remove_reference_t<
			decltype(*std::declval<Iterator>())
		> >;
		static_assert(std::is_same_v<AlphabetType, IteratorReturnType>
				|| std::is_same_v<std::make_signed_t<AlphabetType>, IteratorReturnType>
				|| std::is_same_v<std::make_unsigned_t<AlphabetType>, IteratorReturnType>
				, ""
		);
	}


	///
	/// \brief Making and initialize code containers
	/// \tparam CodeContainer encode/decode container
	/// \return inited CodeContainer object
	///
	template<typename CodeContainer>
	CodeContainer makeCodeContainer() const;
};

}

namespace base_coder
{

template<typename Trait>
template<typename InputIterator>
size_t BaseCoder<Trait>::encodeSize(View<InputIterator> inputView) const
{
	checkIteratorType<InputIterator>();

	constexpr size_t encodeInputSize = EncodeInput{}.size();
	constexpr size_t encodeOutputSize = EncodeOutput{}.size();

	const size_t inputViewSize = inputView.size();
	size_t size = ( (inputViewSize / encodeInputSize)
			+ (inputViewSize % encodeInputSize != 0) )
			* encodeOutputSize;
	return size;
}

template<typename Trait>
template<typename Container>
size_t BaseCoder<Trait>::encodeSize(const Container &container) const
{
	return encodeSize(makeConstView(container));
}

template<typename Trait>
template<typename InputIterator>
size_t BaseCoder<Trait>::decodeSize(View<InputIterator> inputView) const
{
	checkIteratorType<InputIterator>();

	constexpr size_t decodeInputSize = DecodeInput{}.size();
	constexpr size_t decodeOutputSize = DecodeOutput{}.size();

	const size_t inputViewSize = inputView.size();
	const size_t countBlocks = inputViewSize / decodeInputSize;
	const size_t lastBlockPosition = (countBlocks ? countBlocks - 1 : 0)
			* decodeInputSize;

	InputIterator it = inputView.begin();
	if constexpr (std::is_same_v<
		typename std::iterator_traits<InputIterator>::iterator_category
		, std::random_access_iterator_tag>)
	{
		it += lastBlockPosition;
	}
	else
	{
		for (auto i = lastBlockPosition; i != 0; --i)
		{
			++it;
		}
	}

	size_t size = ((inputViewSize ? inputViewSize : decodeInputSize)
			/ decodeInputSize - 1) * decodeOutputSize;

	// decode last block
	decode(View<InputIterator>{ it, inputView.end() }
			, makeFakeIterator(size, [&size](){ ++size; }));

	return size;
}

template<typename Trait>
template<typename Container>
size_t BaseCoder<Trait>::decodeSize(const Container &container) const
{
	return decodeSize(makeConstView(container));
}


template<typename Trait>
template<typename InputIterator, typename OutputIterator>
void BaseCoder<Trait>::encode(View<InputIterator> inputView
		, OutputIterator outputIterator) const
{
	checkIteratorType<InputIterator>();
	//checkIteratorType<OutputIterator>();

	EncodeInput encodeInput = makeCodeContainer<EncodeInput>();
	uint8_t encodeInputIndex = 0;
	for (auto i : inputView)
	{
		encodeInput[encodeInputIndex++] = i;

		if (encodeInputIndex == encodeInput.size())
		{
			encodeInputIndex = 0;
			EncodeOutput encodeOutput = coreEncode(encodeInput);
			std::copy(encodeOutput.begin(), encodeOutput.end(), outputIterator);
			encodeInput = makeCodeContainer<EncodeInput>();
		}
	}
	if (encodeInputIndex)
	{
		EncodeOutput encodeOutput = coreEncode(encodeInput);
		std::copy(encodeOutput.begin(), encodeOutput.end(), outputIterator);
	}
}

template<typename Trait>
template<typename Container, typename OutputIterator>
void BaseCoder<Trait>::encode(const Container &container
		, OutputIterator outputIterator) const
{
	encode(makeView(std::cref(container)), outputIterator);
}

template<typename Trait>
template<typename InputIterator, typename OutputIterator>
void BaseCoder<Trait>::decode(View<InputIterator> inputView
		, OutputIterator outputIterator) const
{
	checkIteratorType<InputIterator>();
	//checkIteratorType<OutputIterator>();

	DecodeInput decodeInput = makeCodeContainer<DecodeInput>();
	uint8_t decodeInputIndex = 0;
	for (auto i : inputView)
	{
		decodeInput[decodeInputIndex++] = i;

		if (decodeInputIndex == decodeInput.size())
		{
			decodeInputIndex = 0;
			DecodeOutput decodeOutput = coreDecode(decodeInput);
			std::copy_if(decodeOutput.begin(), decodeOutput.end(), outputIterator
					, [](auto i) { return i != 0; });
			decodeInput = makeCodeContainer<DecodeInput>();
		}
	}
	if (decodeInputIndex)
	{
		DecodeOutput encodeOutput = coreDecode(decodeInput);
		std::copy_if(encodeOutput.begin(), encodeOutput.end(), outputIterator
				, [](auto i) { return i != 0; });
	}
}

template<typename Trait>
template<typename Container, typename OutputIterator>
void BaseCoder<Trait>::decode(const Container &container
		, OutputIterator outputIterator) const
{
	decode(makeView(std::cref(container)), outputIterator);
}

// private

template<typename Trait>
typename BaseCoder<Trait>::EncodeOutput
BaseCoder<Trait>::coreEncode(EncodeInput data) const
{
	constexpr Buffer BASE_BIT_MASK = uppedMask<indexBitSize>;

	EncodeOutput output;
	Buffer buffer = 0;
	auto dataView = View<decltype(data.begin())>{ data.begin(), data.end() - 1 };
	for (auto i : dataView)
	{
		buffer += i;
		buffer <<= CHAR_BIT;
	}
	buffer += *(data.end() - 1);

	for (std::size_t i = 0; i < output.size(); ++i)
	{
		const std::size_t shift = indexBitSize * (output.size() - i - 1);
		Buffer currentMask = BASE_BIT_MASK << shift;

		const Buffer resultIndex = (buffer & currentMask) >> shift;
		output[i] = (resultIndex != 0)
				? alphabet[resultIndex]
				: pad;
	}

	return output;
}

template<typename Trait>
typename BaseCoder<Trait>::DecodeOutput
BaseCoder<Trait>::coreDecode(DecodeInput data) const
{
	constexpr Buffer BASE_BIT_MASK = uppedMask<CHAR_BIT>;

	auto getIndex = [](auto value) -> uint8_t
	{
		if (value == pad)
		{
			return 0;
		}
		size_t index = 0;
		for (; index < alphabetSize; ++index)
		{
			if (alphabet[index] == value)
			{
				return index;
			}
		}
		return index;
	};

	Buffer buffer = 0;
	for (size_t i = 0; i < data.size(); ++i)
	{
		buffer += static_cast<Buffer>( getIndex(data[i]) )
				<< (indexBitSize * (data.size() - i - 1));
	}

	auto output = makeCodeContainer<DecodeOutput>();
	for (size_t i = 0; i < output.size(); ++i)
	{
		const size_t shiftValue = CHAR_BIT * (output.size() - i - 1);
		output[i] = static_cast< std::remove_reference_t<decltype(output[i])> >(
				(buffer & (BASE_BIT_MASK << shiftValue)) >> shiftValue
		);
	}
	return output;
}

template<typename Trait>
template<typename CodeContainer>
CodeContainer BaseCoder<Trait>::makeCodeContainer() const
{
	CodeContainer container;
	for (auto &i : container)
	{
		i = std::remove_reference_t<decltype(i)>{};
	}
	return container;
}

} // namespace base_coder

//#include <vector>
//#include <functional>

//void func()
//{
//	std::vector<uint8_t> value;
//	base_coder::templates::BaseCoder< base_coder::templates::Base64Trait > baseCoder;
//	baseCoder.encode(
//			base_coder::templates::makeView(std::cref(value))
//			, base_coder::templates::makeView(std::ref(value))
//	);
//}

#endif // BASECODER_HPP
