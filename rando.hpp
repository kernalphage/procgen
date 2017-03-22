#pragma once
#include <gradient_noise/gradient_noise.hpp>
#include <complex>

namespace rando{
using icomplex = std::complex<float>; 	
const float pi = std::acos(-1);
    std::random_device rd;
    std::mt19937 gen;
    std::weibull_distribution<> d{1,1.5};
    std::uniform_real_distribution<float> u{0.0f,2*pi};
    std::uniform_real_distribution<float> out{-1,1.0f};
    gnd::gradient_noise<float,3> chaos_dimension;

    template< class Sseq > 
    Sseq init_rand(Sseq seed)
    {
	if(seed == 0)
	{
		seed = rd();
	}
	chaos_dimension.seed(seed);
	gen.seed(seed);
	return seed;
    }
    float next_position(icomplex xy,float t)
    {
	    return out(gen);
    }
    float next_position(const float x,const float y,const float t =0)
    {
		return out(gen);
    }
    float next_smooth()
    {
    	return d(gen);
    }
    float next_unit()
    {
    	return u(gen);
    }
    float next_range()
    {
    	return out(gen);
    }
}

