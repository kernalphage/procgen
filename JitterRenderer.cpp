//
// Created by matt on 4/1/17.
//

#include "JitterRenderer.hpp"
#include "rando.hpp"
#include "helper.hpp"
#include <chrono>

template<typename T>
void JitterRenderer::render_spline(const bezier<T> &b, vector<int> &accumulator) {

// map -1, 1 to m_s.getWidth(), m_s.getHeight().
// TODO: worry about aspect ratio
    float ishwidth = m_s.getHeight() * .5f;
    int curve_count = b.size() / 2 - 1;

    for (int i = 0; i < curve_count; i++) {
// for each (0,1) curve, plot one per pixel
        float curve_len = bezier<icomplex>::arc_length(b, i);

        int pixels_in_curve = curve_len * ishwidth  * m_s.getSupersample();
        pixels_in_curve = min(pixels_in_curve, m_s.getWidth() * 10);
        pixels_in_curve = max(pixels_in_curve, 1);
        float dt = 1.0f / pixels_in_curve;  // might need fudge
        for (float t = 0; t < 1; t += dt) {
            icomplex j = b.sample(i + t);
            int ex = (int) (j.real() * ishwidth  + m_s.getWidth() / 2);
            int wy = (int) (j.imag() * ishwidth  + m_s.getHeight() / 2);

            if (helper::oob(ex, 0, m_s.getWidth() - 1) || helper::oob(wy, 0, m_s.getHeight() - 1)) {
                continue;
            }

            accumulator[ex + wy * m_s.getWidth()]++;
        }
    }
}


png::image<png::rgb_pixel> JitterRenderer::tonemap() {

    bezier<fcolor> colormap(m_s.getColors());
    // Color is also a bezier curve, but it's still a bit messy
    float max_sample = max(colormap.size() / 2 - 1, 1);
    // HDR, with some fudge
    float max_energy = pow((float) max_val, m_s.getGamma());
    png::image<png::rgb_pixel> image(m_s.getWidth(), m_s.getHeight());
    for (size_t y = 0; y < image.get_height(); ++y) {
        for (size_t x = 0; x < image.get_width(); ++x) {
            float energy = pow((float) m_accumulator[x + y * m_s.getWidth()], m_s.getGamma()) / max_energy;

            auto color = (png::rgb_pixel) colormap.sample(energy * max_sample);
            image.set_pixel(x, y, color);
        }
    }
    return image;
}

void JitterRenderer::render() {

    rando::init_rand(m_s.getSeed());
    cout << "Seeding with " << m_s.getSeed() << endl;

    // Generate base model
    vector<icomplex> pts(m_s.getNum_pts());
    for(int i=0; i < pts.size(); i++){
        pts[i] = m_s.getGenerator()(i, pts.size());
    }


    bezier<icomplex> b = bezier<icomplex>(pts);
    // Reset canvas
    m_accumulator.resize( m_s.getWidth() * m_s.getHeight() );
    std::fill(m_accumulator.begin(), m_accumulator.end(), 0);

    cout << "[";
    cout.flush();
    int stepsize = max(1, (m_s.getIterations() / 100));
    auto jitter = m_s.getJitter();

    // Simulation phase
    for (int i = 0; i < m_s.getIterations(); i++) {
        render_spline(b, m_accumulator);
        //movers::g_t += m_s.getDt();
        b.jiggle( jitter );
        if ((i % stepsize) == 0) {
            cout << "#";
            cout.flush();
        }
    }
    cout << "]" << endl;

    max_val = *std::max_element(m_accumulator.begin(), m_accumulator.end());

}

png::image<png::rgb_pixel> JitterRenderer::generate_image() {

    // Rendering begins
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    if(m_s.getNeedsRender()){
        render();
    }

    if(m_s.getNeedsTonemap()){
        m_image = tonemap();
    }

    m_s.reset();

    // keep track of time elapsed
    end = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
    // TODO(matt): get elapsed time out of here somehow
    return m_image;
}
