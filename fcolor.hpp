#ifndef PROCGEN_FCOLOR_HPP
#define PROCGEN_FCOLOR_HPP

#include "png++/png.hpp"


class fcolor {
public:

    // Overload + operator to add two fcolor objects.
    fcolor operator+(const fcolor &b) {
        fcolor ret;
        ret.r = this->r + b.r;
        ret.g = this->g + b.g;
        ret.b = this->b + b.b;
        return ret;
    }
    bool operator==(const fcolor& other) const{
        return (r == other.r) && (b == other.r) && (g == other.g);
    }
    // Overload + operator to add two fcolor objects.
    fcolor operator*(const float b) const{
        fcolor ret;
        ret.r = this->r * b;
        ret.g = this->g * b;
        ret.b = this->b * b;
        return ret;
    }

    explicit operator png::rgb_pixel() {
        unsigned char _r, _g, _b;
        _r = std::min<unsigned char>(255, std::max<unsigned char>(0, r));
        _g = std::min<unsigned char>(255, std::max<unsigned char>(0, g));
        _b = std::min<unsigned char>(255, std::max<unsigned char>(0, b));
        return png::rgb_pixel{_r, _g, _b};
    }

    float r, g, b;
};

inline fcolor operator*(const float &b, const fcolor &a){
    fcolor ret;
    ret.r = a.r * b;
    ret.g = a.g * b;
    ret.b = a.b * b;
    return ret;
}

#endif //PROCGEN_FCOLOR_HPP