#ifndef BASECODER_TEST_HPP
#define BASECODER_TEST_HPP

#include <gtest/gtest.h>

namespace std
{

template<typename CharType>
inline bool operator==(std::basic_string_view<CharType> sv1
		, std::basic_string_view<CharType> sv2)
{
	if (sv1.size() == sv2.size())
	{
		auto it2 = sv2.begin();
		for(auto i1 : sv1)
		{
			if(i1 == *it2)
			{
				++it2;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

} // namespace std

namespace base_coder
{

namespace test
{

class BaseCoderTest : public ::testing::Test
{
protected:
	void SetUp() override
	{}

	void TearDown() override
	{}

protected:
	const std::vector<std::string> refereceData = {
			""
			,"f"
			, "fo"
			, "foo"
			, "foob"
			, "fooba"
			, "foobar"
			, "Man is distinguished, not only by his reason, but by this "
			"singular passion from other animals, which is a lust of the "
			"mind, that by a perseverance of delight in the continued and "
			"indefatigable generation of knowledge, exceeds the short "
			"vehemence of any carnal pleasure."
	};
	const std::vector<std::string> refereceEncodedDataBase64 = {
			""
			, "Zg=="
			, "Zm8="
			, "Zm9v"
			, "Zm9vYg=="
			, "Zm9vYmE="
			, "Zm9vYmFy"
			, "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0"
			"aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1"
			"c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0"
			"aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdl"
			"LCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="
	};
	const std::vector<std::string> refereceEncodedDataBase32 = {
			""
			, "MY======"
			, "MZXQ===="
			, "MZXW6==="
			, "MZXW6YQ="
			, "MZXW6YTB"
			, "MZXW6YTBOI======"
	};
	const std::vector<std::string> refereceEncodedDataBase16 = {
			""
			, "66"
			, "666F"
			, "666F6F"
			, "666F6F62"
			, "666F6F6261"
			, "666F6F626172"
	};
};

}
}

#endif // BASECODER_TEST_HPP
