#ifndef PROCGEN_RANDO_HPP
#define PROCGEN_RANDO_HPP
#include <random>
#include <complex>

struct rando {
    using icomplex = std::complex<float>;
    static const constexpr float pi = 3.1415f;
    static std::random_device rd;
    static std::mt19937 gen;
    static std::weibull_distribution<float> d;
    static std::uniform_real_distribution<float> u;
    static std::uniform_real_distribution<float> out;

    template<class Sseq>
    static Sseq init_rand(Sseq seed) {
        if (seed == 0) {
            seed = rd();
        }
        gen.seed(seed);

        d = std::weibull_distribution<float>{1, 1.5f};
        u = std::uniform_real_distribution<float>{0.0f, 2 * pi};
        out = std::uniform_real_distribution<float>{-1, 1.0f};
        return seed;
    }

    static float next_position(icomplex xy, float t) {
        return out(gen);
    }

    static float next_position(const float x, const float y, const float t = 0) {
        return out(gen);
    }

    static float next_unit() {
        return u(gen);
    }

    static float next_range() {
        return out(gen);
    }
};

#endif // PROCGEN_RANDO_HPP