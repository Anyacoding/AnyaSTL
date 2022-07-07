#include <iostream>
#include <vector>
#include <map>
#include "anya_alloc.h"

int main()
{
	Anya::allocator<int> alloc;
	auto ptr = alloc.allocate(8);
	*ptr = 114154;
	std::cout << *ptr << std::endl;
	std::map<int, int> dp;
	return 0;
}