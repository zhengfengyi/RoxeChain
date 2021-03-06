//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <tuple>

// template <class... Types> class tuple;

// template <class Tuple, __tuple_assignable<Tuple, tuple> >
//   tuple & operator=(Tuple &&);

// This test checks that we do not evaluate __make_tuple_types
// on the array when it doesn't match the size of the tuple.

#include <array>
#include <tuple>

// Use 1256 to try and blow the template instantiation depth for all compilers.
typedef std::array<char, 1256> array_t;
typedef std::tuple<array_t> tuple_t;

int main()
{
    array_t arr;
    tuple_t tup;
    tup = arr;
}
