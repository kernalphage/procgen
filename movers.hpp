#pragma once
#include <cmath>

namespace movers
{
	float g_t =0;
	using icomplex = std::complex<float>;
	// The farther away, the more jiggle you have
	auto smoke_rise(float base_energy, float movescale)
	{
		return [=](icomplex i){
			float dist = abs(norm(i) - 1.0f) + base_energy;
			//float dist = log(abs(norm(i)-1) + 1) + base_energy;
 			icomplex dp = movescale * dist * icomplex(rando::next_range(), rando::next_range());

			return i + dp;
		};
	}

	//things in the center of the screen move more
    auto center_out(float movescale, float falloff)
	{
		return [=](icomplex i){
		float dist = 1/(falloff * abs(i.real())+1);
		auto norm = (i + icomplex(1,1) )* 500.0f ;
		icomplex dp = movescale * dist * icomplex(rando::next_position(norm, 1,g_t), rando::next_position(norm,100,g_t));
		return i + dp;
	};
	}

}
