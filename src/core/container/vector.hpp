//
// Created by Anya on 2023/3/12.
//

#ifndef ANYA_STL_VECTOR_HPP
#define ANYA_STL_VECTOR_HPP

#include "allocator/memory.hpp"
#include <type_traits>

namespace anya {

template<class T,
         class Allocator = anya::allocator<T>>
class vector {
private:
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                  "std::vector must have a non-const, non-volatile value_type");
    static_assert(std::is_same<typename Allocator::value_type, T>::value,
                  "std::vector must have the same value_type as its allocator");

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type  = Allocator;
};

}

#endif //ANYA_STL_VECTOR_HPP
