#pragma once
namespace rando{
	
const float pi = std::acos(-1);
    std::random_device rd;
    std::mt19937 gen;
    std::weibull_distribution<> d{1,1.5};
    std::uniform_real_distribution<float> u{0.0f,2*pi};
    std::uniform_real_distribution<float> out{-1,1.0f};

    template< class Sseq > 
    Sseq init_rand(Sseq seed)
    {
	if(seed == 0)
	{
		seed = rd();
	}
	gen.seed(seed);
	return seed;
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

