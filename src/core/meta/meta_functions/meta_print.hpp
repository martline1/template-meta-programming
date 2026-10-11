#pragma once

#include <iostream>
#include <typeinfo>

namespace core::meta
{


template<typename T>
struct meta_print {
	meta_print() = delete;

	static void print() {
		std::cout << "Typename: " << typeid(T).name() << "\n";
	}
};


template<
	// Deducted from parameters:
	template<typename...> typename Container,
	typename... Args
>
struct meta_print<Container<Args...>> {
	meta_print() = delete;

	static void print() {
		std::cout << "Container typename: " << typeid(Container<Args...>).name()
			<< "\n"
			<< "Element typenames: ";

		bool first = true;

		auto print_element = [&](const char* name) {
			if (!first) std::cout << ", ";
			std::cout << name;
			first = false;
		};

		std::cout << "<";
		((print_element(typeid(Args).name())), ...);
		std::cout << ">\n\n";
	}
};


} // namespace core::meta
