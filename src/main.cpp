#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include "./core/allocator/anya_alloc.h"


clock_t start, end;

int main()
{
	Anya::allocator<int> my_alloc;
	std::allocator<int> std_alloc;
	start = clock();
	auto ptr = my_alloc.allocate(1e8);
	my_alloc.deallocate(ptr, 1e8);
	end = clock();
	std::cout << (float)(end - start) / CLOCKS_PER_SEC << " s" << std::endl;

	start = clock();
	ptr = std_alloc.allocate(1e8);
	std_alloc.deallocate(ptr, 1e8);
	end = clock();
	std::cout << (float)(end - start) / CLOCKS_PER_SEC << " s" << std::endl;
	return 0;
}