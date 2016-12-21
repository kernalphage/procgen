#pragma once 
#include <ostream>
#include <tuple>
namespace helper{

// helper function to print_tuple a tuple of any size
template<class Tuple, std::size_t N>
struct TuplePrinter {
    static void print_tuple(const Tuple& t, std::ostream& os) 
    {
        TuplePrinter<Tuple, N-1>::print_tuple(t, os);
        os << "|" << std::get<N-1>(t);
    }
};
 
template<class Tuple>
struct TuplePrinter<Tuple, 1> {
    static void print_tuple(const Tuple& t, std::ostream& os) 
    {
         os << std::get<0>(t);
    }
};
 
template<class... Args>
void print_tuple(std::ostream& os, const std::tuple<Args...>& t) 
{
     TuplePrinter<decltype(t), sizeof...(Args)>::print_tuple(t,os);
}
// end helper function

}
