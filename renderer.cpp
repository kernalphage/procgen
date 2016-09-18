
#include <iostream>
#include <string>
#include <complex>
#include <cmath>
#include "bezier.hpp"
#include "png++/png.hpp"

#include <cmath>
using namespace std;
using namespace png;
using icomplex = complex<float>;

const float pi = std::acos(-1);
namespace rando{
	std::random_device rd;
    std::mt19937 gen(rd());
 
    std::weibull_distribution<> d{1,1.5};
    std::uniform_real_distribution<float> u{0.0f,2*pi};
    std::uniform_real_distribution<float> out{.5f,1.0f};
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

template<typename T>
auto lerper (T min, T max, int divisor)
{ 
	return [=](int t){float tt = ((float)t)/divisor; return (T)(min + (max-min)*tt) ;};
}



template <typename T>
void render(bezier<T> b, float width, png::image<png::rgb_pixel> &image,  png::rgb_pixel px)
{
	float ishwidth = width * .3f;
	
	for(float i=0; i < b.length() / 2 - 1; i+=1/500.0)
	{
		icomplex j = b.sample(i);

		try
		{
			int ex = (int) (j.real()*ishwidth + width/2);
			int wy = (int) (j.imag()* ishwidth + width/2);
			if(image.get_pixel(ex,wy).red == 0)
				image.set_pixel( ex, wy, px);
		}
		catch(std::out_of_range e )
		{

		}
	}
}

int main(int ac, char* av[])
{

	size_t num_pts = 64;
	vector<icomplex> pts(num_pts);
	float di = pi*2	 / (num_pts-2);
	int i =0;
	std::generate(pts.begin(), pts.end(), [&]{
		float theta = di * i++;
		return icomplex(sin( theta), cos(theta));
	});

	auto jiggle_height = [](icomplex i){
		float height = 1 - (i.imag() + 1) /2.0f;
		auto dp = .04f * height * rando::next_range() * polar(1.0f, rando::next_unit() );
		return i + dp;
	};
    bezier<icomplex> b = bezier<icomplex>( pts );
    
    int width = 1024;
	png::image<png::rgb_pixel> image(width, width);

	for (size_t y = 0; y < image.get_height(); ++y)
	 {
	     for (size_t x = 0; x < image.get_width(); ++x)
	     {
	         image[y][x] = png::rgb_pixel(0,0,0);
	         // non-checking equivalent of image.set_pixel(x, y, ...);
	     }
	}
	int steps = 500;
	auto red =   lerper<unsigned char>(250,150, steps );
	auto green = lerper<unsigned char>(150,5, steps );
	auto blue =  lerper<unsigned char>(20, 0, steps );
	for(int i=0; i < steps; i++)
	{
		render(b,width, image, {red(i), green(i), blue(i)});
		b.jiggle(jiggle_height);
	}

	cout<<pts.back()<<endl;
	cout<<b.sample(num_pts/2)<<endl;
	image.write("output.png");
}
