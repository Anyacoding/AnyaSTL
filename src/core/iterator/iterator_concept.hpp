//
// Created by Anya on 2023/3/12.
//

#ifndef ANYA_STL_ITERATOR_CONCEPT_HPP
#define ANYA_STL_ITERATOR_CONCEPT_HPP

#include "iterator.hpp"

namespace anya {

template<typename Iter>
concept random_access_iterator =
    requires(Iter i, const Iter j, const anya::iter_difference_t<Iter> n) {
        { i += n } -> std::same_as<Iter&>;
        { j +  n } -> std::same_as<Iter>;
        { n +  j } -> std::same_as<Iter>;
        { i -= n } -> std::same_as<Iter&>;
        { j -  n } -> std::same_as<Iter>;
        {  j[n]  } -> std::same_as<anya::iter_reference_t<Iter>>;
    };
}

#endif //ANYA_STL_ITERATOR_CONCEPT_HPP
