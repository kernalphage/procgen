#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <complex>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

using namespace std;
using namespace glm;


class bezier {


public:

    struct samp {
        vec3 p;
        vec3 n;
    };

    bezier(std::vector<vec3> init)
            : m_ctrl(init) {

    }

    void append_control(std::vector<vec3> l) {
        m_ctrl.insert(m_ctrl.end(), l.begin(), l.end());
    }

    //how do I do this, in haskell it'd be  :: T -> T
    template<typename Functor>
    void jiggle(Functor wiggle) {
        for (auto &i : m_ctrl) {
            i = wiggle(i);
        }
    }

    int size() const {
        return m_ctrl.size();
    }


    float arc_length(size_t i) const{
        float len{};
        if (i < 0 || (i + 2) > m_ctrl.size()) {
            return len;
        }

        float chord = length(m_ctrl[i] - m_ctrl[i + 2]);
        float ctrl_net = length(m_ctrl[i] - m_ctrl[i + 1]) + length(m_ctrl[i + 2] - m_ctrl[i + 1]);
        len += (chord + ctrl_net) / 2.0f;
        return len;
    }

    vec3 sample(float t) const {
        size_t start = 0;

        // chain to further pairs
        if (t < 0) {
            return m_ctrl.front();
        }
        if (t > 1) {
            double s = 0;
            t = modf(t, &s);
            start = ((size_t) s) * 2;
            if (start + 2 > m_ctrl.size()) {
                return m_ctrl.back();
            }
        }
        float inv_t = (1 - t);
        return inv_t * inv_t * m_ctrl[start] + 2 * inv_t * t * m_ctrl[start + 1] + t * t * m_ctrl[start + 2];
    }


    void make_samples(vector<vec3> &list, int iterations){
        if(iterations < 0) return;

        float dt = (float) m_ctrl.size() / iterations;
        for(int i=0; i < iterations; i++)
        {
            float t = i * dt;
            list.push_back( sample(t) );
        }

    };


private:

    vector<vec3> m_ctrl;
};
