#include <chrono>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "png++/png.hpp"
#include "rando.hpp"
#include "movers.hpp"
#include "bezier.hpp"
#include "fcolor.hpp"
#include "helper.hpp"
#include "RenderSettings.hpp"
#include "JitterRenderer.hpp"

using namespace std;
using namespace std::chrono;
using namespace png;
using icomplex = complex<float>;

render_settings g_s{};

void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        auto cur = argv[i];
        if (strcmp("-d", cur) == 0) {         // Dimensions
            g_s.setWidth(atoi(argv[++i]));
            g_s.setHeight(atoi(argv[++i]));
        } else if (strcmp("-s", cur) == 0) {  // seed
            g_s.setSeed(atol(argv[++i]));
        } else if (strcmp("-i", cur) == 0) {   // Iterations
            g_s.setIterations(atoi(argv[++i]));
        } else if (strcmp("-g", cur) == 0) {    // Gamma
            g_s.setGamma((float) atof(argv[++i]));
        } else if (strcmp("-p", cur) == 0) {    // control points
            g_s.setNum_pts((size_t) atoi(argv[++i]));
        } else if (strcmp("-a", cur) == 0) {    // Amplitude
            g_s.setAmplitude((float) atof(argv[++i]));
        } else if (strcmp("--ppp", cur) == 0) {     // points per pixel
            g_s.setSupersample((float) atof(argv[++i]));
        } else if (strcmp("--dt", cur) == 0) {      // delta_time
            cout << "--dt" << endl;
            g_s.setDt((float) atof(argv[++i]));
        } else {
            cout << "Could not parse " << cur << endl;
        }
    }
}

int main(int ac, char *av[]) {
    parse_args(ac, av);

// Possible generator methods
    auto circular = [&] (int cur, int max){
        float di = rando::pi * 2 / (max - 2);
        float theta = di * cur;
        return icomplex(sin(theta), cos(theta)) * .8f;
    };

    auto linear = [&] (int cur, int max){
        float di_linear = 4.0f / (max - 2);
        return icomplex(cur * di_linear - 2.0f, 0);
    };
// Possible jiggle types;
    auto jiggle_height = [=](icomplex ic) {
        float height = 1 - (ic.imag() + 1) / 2.0f + .0001;
        icomplex dp = .01f * height * rando::next_position(ic.real(), ic.imag(), 2.0f) *
                      polar(rando::next_range(), rando::next_unit());
        return ic + dp;
    };

    float move_scale = g_s.getAmplitude();
    float base_energy = .09f; // .05f;
    auto smoke_rise = movers::smoke_rise(base_energy, move_scale);
    float falloff = 30.f;

    auto center_out = [=](icomplex i_center) {
        float dist = 1 / (falloff * abs(i_center.real()) + 1);
        auto norm = (i_center + icomplex(1, 1)) * 500.0f;
        icomplex dp = move_scale * icomplex(rando::next_position(norm, 1), rando::next_position(norm, 100));
        return i_center + dp;
    };

    // Tone mapping
    vector<fcolor> fcolors{
            {0.f,   0.f,   0.f},
            {255.f, 20.f,  0.f},
            {255.f, 250.f, 70.f}
    };

    g_s.setColors(fcolors);
    g_s.setGenerator(circular);
    g_s.setJitter(center_out);

    JitterRenderer r;
    r.setSettings(g_s);

    auto image = r.generate_image();

    const char *fmt = "output_%h%d_%H-%M-%S.png";
    char buf[80];
    struct tm *timeinfo;
    auto curtime = system_clock::to_time_t(system_clock::now());
    timeinfo = localtime(&curtime);
    strftime(buf, 80, fmt, timeinfo);
    image.write(buf);

    std::fstream log_handle;
    log_handle.open("log.txt", ios::app | ios::out);

    log_handle <<std::string(buf) << "|" << g_s.pickle() << endl;
}
	
