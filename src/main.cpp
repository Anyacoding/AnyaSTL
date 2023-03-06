#include <iostream>
#include <memory>
#include <ctime>
#include <vector>
#include "allocator/anya_alloc.hpp"

int main() {
    clock_t start, end;
    anya::allocator<int> my_alloc;
    std::allocator<int> std_alloc;
    start = clock();
    auto ptr = my_alloc.allocate(1e9);
    my_alloc.deallocate(ptr, 1e9);
    end = clock();
    std::cout << (float)(end - start) / CLOCKS_PER_SEC << " s" << std::endl;

    start = clock();
    ptr = std_alloc.allocate(1e9);
    std_alloc.deallocate(ptr, 1e9);
    end = clock();
    std::cout << (float)(end - start) / CLOCKS_PER_SEC << " s" << std::endl;

    anya::allocator<std::vector<int>> vec_alloc;
    auto vec = vec_alloc.allocate(1);
    vec_alloc.construct(vec, 10, 2);
    for (auto num : *vec) {
        std::cout << num << std::endl;
    }
    return 0;
}



