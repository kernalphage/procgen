#include <chrono>
#include <cmath>
#include <cmath>
#include <complex>
#include <ctime>
#include <iostream>
#include <string>
#include "png++/png.hpp"
#include "bezier.hpp"
#include "fcolor.hpp"
using namespace std;
using namespace png;
using icomplex = complex<float>;

const float pi = std::acos(-1);
namespace rando{
	std::random_device rd;
    std::mt19937 gen(rd());
 
    std::weibull_distribution<> d{1,1.5};
    std::uniform_real_distribution<float> u{0.0f,2*pi};
    std::uniform_real_distribution<float> out{-1,1.0f};
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


template<typename T, typename T1, typename T2> 
bool oob(const T t,const T1 lower,const T2 upper)
{
	return t < lower || t > upper; 
}


template <typename T>
void render_spline(bezier<T> b, int width, int height, vector<int> &accumulator)
{
	float ishwidth = height * .5f;
	
	for(float i=0; i < b.size() / 2 - 1; i+=1/500.0)
	{
		icomplex j = b.sample(i);
		int ex = (int) (j.real() * ishwidth + width/2);
		int wy = (int) (j.imag() * ishwidth + height/2);

		if(oob (ex, 0, width-1) || oob(wy, 0, height-1) )
		{
			continue;
		}

		accumulator[ex + wy * width]++;
	}
}

void render_particle(int px, int py, const int width, const int height, vector<int>& accumulator){
	const float damping = .8f;
	const float accel = .001;
	const int lifetime = 5000;

	float vx = 0, vy = 0; 
	float ax = 0, ay = 0;
	float fpx = px, fpy = py;

	for(int i=0; i < lifetime; i++)
	{
		ax = ax * damping + rando::next_range() * accel;
		ay = ay * damping + rando::next_range() * accel;
		vx += ax;
		vy += ay; 
		fpx += vx;
		fpy += vy;
		int ex = (int) (fpx);
		int wy = (int) (fpy);
		if(oob (ex, 0, width-1) || oob(wy, 0, height-1) )
		{
			break;
		}
		accumulator[ex + wy * width]++;
	}

}

/// follow the leader? one or a dozen 'flyers' take a path, everyone else flocks and take the path? 
// centroids?
png::image<png::rgb_pixel> tonemap(const int width, const int height, int max_val, float gamma, vector<int>&accumulator, const vector<fcolor> & colors){
	
	bezier<fcolor> colormap(colors);

	// HDR, with some fudge
	float max_sample = max(colormap.size() / 2 - 1, 1);
	float max_energy = log(max_val * gamma);
	png::image<png::rgb_pixel> image(width, height);
	for (size_t y = 0; y < image.get_height(); ++y)
	 {
	     for (size_t x = 0; x < image.get_width(); ++x)
	     {
	     	float energy = log(accumulator[x + y*width]) / max_energy;

	     	auto color = (png::rgb_pixel) colormap.sample(energy * max_sample);
	     	image.set_pixel(x,y, color);
	     }
	}
	return image;
}


int main(int ac, char* av[])
{

    	int steps = 29000;
	int width = 4096;
    	int height = 2160;
 	size_t num_pts = 180;
	vector<icomplex> pts(num_pts);
	float di = pi*2	 / (num_pts-2);
	int i =0;
	auto circular = [&]{
		float theta = di * i++;
		return icomplex(sin( theta), cos(theta));
	};


	float di_linear = 4.0f/(num_pts-2);
	auto linear = [&]{
		return icomplex( i++ * di_linear - 2.0f, 0);
	};
	std::generate(pts.begin(), pts.end(), linear);

	auto jiggle_height = [](icomplex i){
		float height = 1 - (i.imag() + 1) / 2.0f + .0001;
		icomplex dp = .01f * height * rando::next_range() * polar(rando::next_range(), rando::next_unit());
		return i + dp;
	};


	float movescale = .029f;
	float base_energy = .25f;
	auto smoke_rise = [=](icomplex i){
		//float dist = abs(norm(i) - 1.0f) + .2f;
		float dist = log(abs(norm(i)-1) + 1) + base_energy;
		icomplex dp = movescale * dist * icomplex(rando::next_range(), rando::next_range());
		return i + dp;
	};

	float falloff = 30.f;
	auto center_out = [=](icomplex i){
		float dist = 1/(falloff * abs(i.real())+1);
		icomplex dp = movescale * dist  * icomplex(rando::next_range(), rando::next_range());
		return i + dp;
	};
    bezier<icomplex> b = bezier<icomplex>( pts );
   ////Rendering begins
   //
    std::chrono::time_point<std::chrono::system_clock> start,end;
    start = std::chrono::system_clock::now();

   vector<int> accumulator(width*height, 0);

	cout<<"[";
	///// Simulation phase
	for(int i=0; i < steps; i++)
	{
		render_spline(b, width, height, accumulator);
		b.jiggle(center_out);
	}
	cout<<"]"<<endl;
    

    // functional pixels?
    /*
    int density = 15;
    for(int i=0; i < width; i += density)
    {
    	for(int j=0; j < height; j+=density)
	 	{
	 		render_particle(i,j,width,height, accumulator);
		}
	}
	*/

	// Tone mapping 
	vector<fcolor> fcolors{
		{  0.f,   0.f,   0.f},
		{255.f, 20.f,   0.f},
		{255.f, 250.f, 70.f}
	};

	vector<fcolor> inspiration{
		{0.f, 0.f, 0.f},
		{10.f, 255.f, 255.f},
		{250.f, 255.f, 255.f},
	};

	vector<fcolor> ocean{
		{255.f, 255.f,   255.f},
//		{10.f, 250.f,   255.f},
//		{80.f, 120.f,   195.f},
		{0.f, 200.f,   255.f},
		{0.f, 0.f, 0.f}
	};

	auto image = tonemap(width, height, steps, .85f,accumulator,  inspiration );

	end = std::chrono::system_clock::now();

	std::chrono::duration<float> elapsed_seconds = end-start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
	const char*  fmt = "output_%h%d_%H-%M-%S.png";
	char buf[80];
	struct tm* timeinfo;
	timeinfo = localtime(& end_time);
	strftime(buf, 80, fmt, timeinfo);
	image.write(buf);
}
