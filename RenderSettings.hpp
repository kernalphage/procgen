//
// Created by matt on 4/1/17.
//

#ifndef PROCGEN_RENDERSETTINGS_HPP
#define PROCGEN_RENDERSETTINGS_HPP
#include "fcolor.hpp"
#include <algorithm>
#include <complex>
#include <chrono>
#include <functional>
#include "RenderSettings.hpp"
#include "helper.hpp"

using namespace std;

using icomplex = complex<float>;
using point_generator = std::function<icomplex(int,int)>;
using point_modifier = std::function<icomplex(icomplex)>;


struct render_settings {
    string pickle() const {
        // TODO(matt): pickle with json or something more stable
        stringstream ss;
        auto params = make_tuple( width, height, seed, iterations, (int) num_pts, gamma,
                                 amplitude, dt);
        helper::print_tuple(ss, params);
        return ss.str();
    }
    bool unpickle(const string& params){

        // TODO(matt): actually unpickle the thing.
        return false;

    }
    // Getters

    int getIterations() const {
        return iterations;
    }

    float getAmplitude() const {
        return amplitude;
    }

    float getDt() const {
        return dt;
    }

    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }
    int getNum_pts() const {
        return num_pts;
    }

    long getSeed() const {
        return seed;
    }
    float getSupersample() const {
        return supersample;
    }
    float getGamma() const {
        return gamma;
    }

    point_modifier getJitter() const {
        return pmod;
    }
    point_generator getGenerator() const{
        return gen;
    }
    const std::vector<fcolor> &getColors() const {
        return colors;
    }

    // Setters


//    todo-matt selectors for generator functions
    void setGenerator(point_generator gen){
        need_render = true;
        render_settings::gen = gen;
    }
    void setJitter(point_modifier mod){
        need_render = true;
        pmod = mod;
    }


    void setAmplitude(float amplitude) {
        if(amplitude != render_settings::amplitude)
            need_render = true;
        render_settings::amplitude = amplitude;
    }

    void setIterations(int iterations) {
        if(iterations != render_settings::iterations)
            need_render = true;
        render_settings::iterations = iterations;
    }

    void setDt(float dt) {
        if(dt != render_settings::dt)
            need_render = true;
        render_settings::dt = dt;
    }

    void setWidth(int width) {
        if(width != render_settings::width)
            need_render = true;
        render_settings::width = width;
    }



    void setHeight(int height) {
        if(height != render_settings::height)
            need_render = true;
        render_settings::height = height;
    }


    void setNum_pts(int num_pts) {
        if(num_pts != render_settings::num_pts)
            need_render = true;
        render_settings::num_pts = num_pts;
    }


    void setSeed(long seed) {
        if(seed != render_settings::seed)
            need_render = true;
        render_settings::seed = seed;
    }

    void setSupersample(float supersample) {
        if(supersample != render_settings::supersample)
            need_render = true;
        render_settings::supersample = supersample;
    }


    void setGamma(float gamma) {
        if(gamma != render_settings::gamma)
            need_tonemap = true;
        render_settings::gamma = gamma;
    }


    void setColors(const std::vector<fcolor> &colors) {
        if(colors != render_settings::colors)
            need_tonemap = true;

        render_settings::colors = colors;
    }

    void reset(){
        need_tonemap = false;
        need_render = false;
    }
    bool getNeedsTonemap(){
        return need_tonemap || need_render;
    }
    bool getNeedsRender(){
        return need_render;
    }




    int iterations = 2900;
    float amplitude = .029f;
    float dt = 0.00001;
    int width = 4096;
    int height = 2160;
    int num_pts = 180;
    long seed = 0;
    float supersample = 1.5f;
    float gamma = .85f;
    std::vector<fcolor> colors;

    point_generator gen;
    point_modifier  pmod;

    bool need_tonemap = true;
    bool need_render = true;
};


#endif //PROCGEN_RENDERSETTINGS_HPP
