#include <cmath>
namespace movers
{
	using icomplex = std::complex<float>;
	auto smoke_rise(float base_energy, float movescale)
	{
		return [=](icomplex i){
		//float dist = abs(norm(i) - 1.0f) + .2f;
		float dist = log(abs(norm(i)-1) + 1) + base_energy;
		icomplex dp = movescale * dist * icomplex(rando::next_range(), rando::next_range());
		return i + dp;
	};
	}

}
