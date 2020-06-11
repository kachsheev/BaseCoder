#include <type_traits>
#include <iterator>

namespace base_coder
{

///
/// \brief The View class
/// \tparam Iterator
///
template<typename Iterator>
class View
{
public:
	///
	/// \brief View
	/// \param begin
	/// \param end
	///
	View(Iterator begin, Iterator end) : itBegin{ begin }, itEnd{ end }
	{}

	///
	/// \brief begin
	/// \return
	///
	Iterator begin() const
	{
		return itBegin;
	}

	///
	/// \brief end
	/// \return
	///
	Iterator end() const
	{
		return itEnd;
	}

	std::size_t size() const
	{
		std::size_t size = 0;
		if constexpr(std::is_same_v<
			typename std::iterator_traits<Iterator>::iterator_category
			, std::random_access_iterator_tag
		>)
		{
			return itEnd - itBegin;
		}
		else
		{
			for (auto &i : *this)
			{
				(void)i;
				++size;
			}
		}
		return size;
	}

private:
	Iterator itBegin; ///<
	Iterator itEnd; ///<
};

///
/// \brief makeView
/// \tparam Container
/// \param container
///
template<typename Container>
auto makeView(Container &container)
{
	return View<decltype(container.begin())>(container.begin(), container.end());
}

///
/// \brief makeConstView
/// \tparam Container
/// \param container
///
template<typename Container>
auto makeConstView(const Container &container)
{
	return View<decltype(container.begin())>(container.begin(), container.end());
}

///
/// \brief makeView
/// \tparam Container
/// \param ref
///
template<typename Container>
auto makeView(std::reference_wrapper<Container> ref)
{
	return View<decltype(ref.get().begin())>(ref.get().begin(), ref.get().end());
}

} // namespace base_coder
