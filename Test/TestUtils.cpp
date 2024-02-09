#include "TestUtils.hpp"

size_t	fail_count = 0;
size_t	success_count = 0;

void	IsEqual(const std::string& a, const std::string& b) {
	if (a.compare(b) != 0) {
		std::cerr << RED << a << " != " << b << "\n" << RESET;
		fail_count += 1;
		return ;
	}
	std::cout << a << " == " << b << "\n";
	success_count += 1;
}

void	IsStringEqual(const std::string& a, const std::string& b) {
	if (a.compare(b) != 0) {
		std::cout << RED << a << " != " << b << "\n" << RESET;
		return ;
	}
	std::cout << GREEN << a << " == " << b << "\n" << RESET;
}

void	IsIntEqual(int a, int b) {
	if (a != b) {
		std::cerr << RED << a << " != " << b << "\n" << RESET;
		return ;
	}
	std::cout << GREEN << a << " == " << b << "\n" << RESET;
}

void	IsVectorEqual(const std::vector<std::string> a, const std::vector<std::string>& b) {
	if (a.size() != b.size()) {
		std::cerr << RED << "size not same\n" << RESET;
	}

	for (size_t i = 0; i < a.size(); ++i) {
		if (a[i].compare(b[i]) != 0)
			std::cout << RED << a[i] << " != " << b[i] << RESET;
		else
			std::cerr << GREEN << a[i] << " == " << b[i] << RESET;
		if (i != a.size() - 1)
			std::cout << ", ";
	}
	std::cout << "\n";
}

