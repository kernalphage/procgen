
#include <iostream>
#include <string>
#include <complex>
#include <cmath>
#include "bezier.hpp"
#include "png++/png.hpp"

#include <cmath>
using namespace std;
using namespace png;


int main(int ac, char* av[])
{

    bezier<float> b = bezier<float>(vector<float>(100, 20));
    float time=0;
    auto sine_jiggle = [&](float t){ return t + sin(t+ (time+=.1f))*10; } ;
 	b.jiggle( sine_jiggle );
	png::image<png::rgb_pixel> image(64, 64);
	int accumulator[64*64];

	image.write("output.png");

	for(float i=0; i < 3; i+=1/50.0)
	{
		float j = b.sample(i);
	}
}
