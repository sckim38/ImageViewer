template <typename T, typename ...>
	struct is_base_of_any
		: std::true_type {};

template <typename T, typename Head, typename ...Rest>
struct is_base_of_any<T, Head, Rest...>
	: std::integral_constant<bool, std::is_base_of<T, Head>::value ||
                                   is_base_of_any<T, Rest...>::value>
{};
