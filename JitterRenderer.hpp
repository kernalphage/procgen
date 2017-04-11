//
// Created by matt on 4/1/17.
//

#ifndef PROCGEN_JITTERRENDERER_HPP
#define PROCGEN_JITTERRENDERER_HPP
#include "bezier.hpp"
#include "RenderSettings.hpp"


class JitterRenderer {
public:


    png::image<png::rgb_pixel> generate_image();

    const render_settings& getSettings(){
        return m_s;
    }
    void setSettings(render_settings settings)
    {
        m_s = settings;
    }

    const int* accumulator(){
        return m_accumulator.data();
    }

private:
    void render();

    template<typename T>
    void render_spline(const bezier<T> &b, vector<int> &accumulator);

    png::image<png::rgb_pixel> tonemap();
    render_settings m_s;

    vector<int> m_accumulator;
    int max_val;
    png::image<png::rgb_pixel> m_image;
};


#endif //PROCGEN_JITTERRENDERER_HPP
