#include <chrono>
#include <cmath>
#include <complex>
#include <iostream>
#include <algorithm>
#include "png++/png.hpp"
#include "rando.hpp"
#include "movers.hpp"
#include "bezier.hpp"
#include "fcolor.hpp"
#include "helper.hpp"

using namespace std;
using namespace png;
using icomplex = complex<float>;

//here there be globals, 'cause why not
int g_iterations = 2900;
int width = 4096;
int height = 2160;
size_t g_num_pts = 180;
long g_seed = 0;
float g_supersample = 1.5f;
float g_gamma = .85f;

float g_amplitude = .029f;
float g_dt = 0.00001;

template<typename T, typename T1, typename T2>
bool oob(const T t, const T1 lower, const T2 upper) {
    return t < lower || t > upper;
}


template<typename T>
void render_spline(const bezier<T> &b, int width, int height, vector<int> &accumulator) {
    // map -1, 1 to width, height.
    // TODO: worry about aspect ratio
    float ishwidth = height * .5f;
    int curve_count = b.size() / 2 - 1;

    for (int i = 0; i < curve_count; i++) {
        // for each (0,1) curve, plot one per pixel
        float curve_len = bezier<icomplex>::arc_length(b, i);

        int pixels_in_curve = curve_len * ishwidth * g_supersample;
        pixels_in_curve = min(pixels_in_curve, width * 10);
        pixels_in_curve = max(pixels_in_curve, 1);
        float dt = 1.0f / pixels_in_curve;  // might need fudge
        for (float t = 0; t < 1; t += dt) {
            icomplex j = b.sample(i + t);
            int ex = (int) (j.real() * ishwidth + width / 2);
            int wy = (int) (j.imag() * ishwidth + height / 2);

            if (oob(ex, 0, width - 1) || oob(wy, 0, height - 1)) {
                continue;
            }

            accumulator[ex + wy * width]++;
        }
    }
}


png::image<png::rgb_pixel>
tonemap(const int width, const int height, int max_val, float gamma, vector<int> &accumulator,
        const vector<fcolor> &colors) {

    bezier<fcolor> colormap(colors);
    // Color is also a bezier curve, but it's still a bit messy
    float max_sample = max(colormap.size() / 2 - 1, 1);
    // HDR, with some fudge
    float max_energy = pow(max_val, gamma);
    png::image<png::rgb_pixel> image(width, height);
    for (size_t y = 0; y < image.get_height(); ++y) {
        for (size_t x = 0; x < image.get_width(); ++x) {
            float energy = pow(accumulator[x + y * width], gamma) / max_energy;

            auto color = (png::rgb_pixel) colormap.sample(energy * max_sample);
            image.set_pixel(x, y, color);
        }
    }
    return image;
}

void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        auto cur = argv[i];
        if (strcmp("-d", cur) == 0) {         // Dimensions
            width = atoi(argv[++i]);
            height = atoi(argv[++i]);
        } else if (strcmp("-s", cur) == 0) {  // seed
            g_seed = atol(argv[++i]);
        } else if (strcmp("-i", cur) == 0) {   // Iterations
            g_iterations = atoi(argv[++i]);
        } else if (strcmp("-g", cur) == 0) {    // Gamma
            g_gamma = (float) atof(argv[++i]);
        } else if (strcmp("-p", cur) == 0) {    // control points
            g_num_pts = (size_t) atoi(argv[++i]);
        } else if (strcmp("-a", cur) == 0) {    // Amplitude
            g_amplitude = (float) atof(argv[++i]);
        } else if (strcmp("--ppp", cur) == 0) {     // points per pixel
            g_supersample = (float) atof(argv[++i]);
        } else if (strcmp("-t", cur) == 0) {        // animation time??
            movers::g_t = (float) atof(argv[++i]);
        } else if (strcmp("--dt", cur) == 0) {      // delta_time
            cout << "--dt" << endl;
            g_dt = (float) atof(argv[++i]);
        } else {
            cout << "Could not parse " << cur << endl;
        }
    }
}

int main(int ac, char *av[]) {
    parse_args(ac, av);
    g_seed = rando::init_rand(g_seed);
    cout << "Seeding with " << g_seed << endl;
    printf("Rendering %dx%d image with size %zd \n", width, height, g_num_pts);
    vector<icomplex> pts(g_num_pts);
    float di = rando::pi * 2 / (g_num_pts - 2);
    int circ_itr = 0;
    auto circular = [&] {
        float theta = di * circ_itr++;
        return icomplex(sin(theta), cos(theta)) * .8f;
    };

    int linear_itr = 0;
    float di_linear = 4.0f / (g_num_pts - 2);
    auto linear = [&] {
        return icomplex(linear_itr++ * di_linear - 2.0f, 0);
    };
    std::generate(pts.begin(), pts.end(), circular);

    auto jiggle_height = [=](icomplex ic) {
        float height = 1 - (ic.imag() + 1) / 2.0f + .0001;
        icomplex dp = .01f * height * rando::next_position(ic.real(), ic.imag(), 2.0f) *
                      polar(rando::next_range(), rando::next_unit());
        return ic + dp;
    };

    float move_scale = g_amplitude;
    float base_energy = .09f; // .05f;
    auto smoke_rise = movers::smoke_rise(base_energy, move_scale);
    float falloff = 30.f;

    auto center_out = [=](icomplex i_center) {
        float dist = 1 / (falloff * abs(i_center.real()) + 1);
        auto norm = (i_center + icomplex(1, 1)) * 500.0f;
        icomplex dp = move_scale * icomplex(rando::next_position(norm, 1), rando::next_position(norm, 100));
        return i_center + dp;
    };
    bezier<icomplex> b = bezier<icomplex>(pts);

    ////Rendering begins
    //
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<int> accumulator(width * height, 0);
    cout << "[";
    cout.flush();
    int stepsize = max(1, (g_iterations / 100));
    ///// Simulation phase
    for (int i = 0; i < g_iterations; i++) {
        render_spline(b, width, height, accumulator);
        movers::g_t += g_dt;
        b.jiggle(smoke_rise);
        if ((i % stepsize) == 0) {
            cout << "#";
            cout.flush();
        }
    }
    cout << "]" << endl;

    // Tone mapping
    vector<fcolor> fcolors{
            {0.f,   0.f,   0.f},
            {255.f, 20.f,  0.f},
            {255.f, 250.f, 70.f}
    };

    vector<fcolor> inspiration{
            {0.f,   0.f,   0.f},
            {10.f,  255.f, 255.f},
            {250.f, 255.f, 255.f},
    };

    vector<fcolor> ocean{
            {255.f, 255.f, 255.f},
//		{10.f, 250.f,   255.f},
//		{80.f, 120.f,   195.f},
            {0.f,   200.f, 255.f},
            {0.f,   0.f,   0.f}
    };

    int max_val = *std::max_element(accumulator.begin(), accumulator.end());

    auto image = tonemap(width, height, max_val, g_gamma, accumulator, inspiration);

    end = std::chrono::system_clock::now();

    std::chrono::duration<float> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
    const char *fmt = "output_%h%d_%H-%M-%S.png";
    char buf[80];
    struct tm *timeinfo;
    timeinfo = localtime(&end_time);
    strftime(buf, 80, fmt, timeinfo);
    image.write(buf);

    std::fstream log_handle;
    log_handle.open("log.txt", ios::app | ios::out);

    auto params = make_tuple(std::string(buf), width, height, g_seed, g_iterations, (int) g_num_pts, g_gamma,
                             g_amplitude, movers::g_t, g_dt, elapsed_seconds.count());
    helper::print_tuple(log_handle, params);
    log_handle << endl;
}
	
