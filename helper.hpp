#pragma once 

#include <ostream>
#include <tuple>
#include <glm/vec3.hpp>

struct VertLayout{
    float x,y,u,v;
};

struct SplineLayout{
    glm::vec3 pos;
};

struct UniformLayout{
    int gamma;
    int max_value;
    int end_color;
};

namespace helper {

    template<typename T, typename T1, typename T2>
    bool oob(const T t, const T1 lower, const T2 upper) {
        return t < lower || t > upper;
    }

// helper function to print_tuple a tuple of any size
    template<class Tuple, std::size_t N>
    struct TuplePrinter {
        static void print_tuple(const Tuple &t, std::ostream &os) {
            TuplePrinter<Tuple, N - 1>::print_tuple(t, os);
            os << "|" << std::get<N - 1>(t);
        }
    };

    template<class Tuple>
    struct TuplePrinter<Tuple, 1> {
        static void print_tuple(const Tuple &t, std::ostream &os) {
            os << std::get<0>(t);
        }
    };

    template<class... Args>
    void print_tuple(std::ostream &os, const std::tuple<Args...> &t) {
        TuplePrinter<decltype(t), sizeof...(Args)>::print_tuple(t, os);
    }
// end helper function

}
