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

//here there be globals, 'cause why not
int g_iterations = 2900;
int width = 4096;
int height = 2160;
size_t num_pts = 180;
long g_seed = 0;
float g_supersample = 1.5f;

const float pi = std::acos(-1);
namespace rando{
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
	int curve_count = b.size() / 2 - 1;

	for(int i=0; i < curve_count; i++)
	{
		// for each (0,1) curve, plot one per pixel
		float curve_len = bezier<icomplex>::arc_length(b, i);
		int pixels_in_curve = curve_len * ishwidth * g_supersample; 
		float dt = 1.0f / pixels_in_curve;  // might need fudge
		for(float t= 0; t < 1; t += dt)
		{
			icomplex j = b.sample(i+t);
			int ex = (int) (j.real() * ishwidth + width/2);
			int wy = (int) (j.imag() * ishwidth + height/2);

			if(oob (ex, 0, width-1) || oob(wy, 0, height-1) )
			{
				continue;
			}

			accumulator[ex + wy * width]++;
		}
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
void parse_args(int argc, char* argv[])
{
	cout<<"argc is " <<argc<<endl;
	for(int i=0; i < argc; i++)
	{
		auto cur = argv[i];
		if(strcmp("-d", cur)==0)
		{
			width = atoi(argv[++i]);
			height = atoi(argv[++i]);
		}
		else if(strcmp("-s", cur)==0)
		{
			g_seed = atol(argv[++i]);
		}
		else if(strcmp("-i", cur)==0)
		{
			g_iterations = atoi(argv[++i]);
		}
		else {
			cout<<"Could not parse " <<cur<<endl;
		}
	}
	cout<<"Finsihed!"<<endl;
}

int main(int ac, char* av[])
{
	cout<<"parsing args"<<endl;
 	parse_args(ac,av);
	g_seed = rando::init_rand(g_seed);
	cout<<"Finished parsing..."<<endl;
	cout<<"Seeding with " << g_seed<<endl;
	printf("Rendering %dx%d image with size %zd \n", width, height, num_pts);
	vector<icomplex> pts(num_pts);
	float di = pi*2	 / (num_pts-2);
	int i =0;
	auto circular = [&]{
		float theta = di * i++;
		return icomplex(sin(theta), cos(theta))*.8f;
	};


	float di_linear = 4.0f/(num_pts-2);
	auto linear = [&]{
		return icomplex( i++ * di_linear - 2.0f, 0);
	};
	std::generate(pts.begin(), pts.end(), circular);

	auto jiggle_height = [](icomplex i){
		float height = 1 - (i.imag() + 1) / 2.0f + .0001;
		icomplex dp = .01f * height * rando::next_range() * polar(rando::next_range(), rando::next_unit());
		return i + dp;
	};


	float movescale = .1;//.029f;
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

	cout<<"["; cout.flush();
	int stepsize = max(1, (g_iterations / 100));
	///// Simulation phase
	for(int i=0; i < g_iterations; i++)
	{
		render_spline(b, width, height, accumulator);
		b.jiggle(center_out);
		if( (i % stepsize)  == 0)
		{
			cout<<"#"; cout.flush();
		} 
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

	auto max_val = 0;
	auto image = tonemap(width, height, g_iterations, .85f,accumulator,  inspiration );

	end = std::chrono::system_clock::now();

	std::chrono::duration<float> elapsed_seconds = end-start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
	const char*  fmt = "output/output_%h%d_%H-%M-%S.png";
	char buf[80];
	struct tm* timeinfo;
	timeinfo = localtime(& end_time);
	strftime(buf, 80, fmt, timeinfo);
	image.write(buf);

	// Log params
	FILE* fp = fopen("output/log.txt", "a");
	fprintf(fp, "%s|%d|%d|%ld|%d|%d|%f\n",buf,width,height,g_seed,g_iterations, (int) num_pts, elapsed_seconds.count() );
	fflush(fp);
	fclose(fp);
}
