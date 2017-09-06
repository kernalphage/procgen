// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include "RenderSettings.hpp"
#include "JitterRenderer.hpp"
#include "rando.hpp"
#include "helper.hpp"
#include "Material.hpp"

render_settings r;
const int im_sz = 64;
constexpr int imDim = im_sz * im_sz;
JitterRenderer render;

TwoTriangles t{};
SplineMat m{};


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

auto two_triangles(void){

    render_settings r;
    r.width = 500;
    r.height = 500;
    r.num_pts = 150;
    r.iterations = 5000;

    r.need_render = true;
    auto circular = [&] (int cur, int max){
        float di = 3.141 * 2 / (max - 2);
        float theta = di * cur;
        return vec3(sin(theta), cos(theta), 0) * .5f;
    };

    auto center_out = [=](vec3 i_center) {
        float dist = .01 * ( length(i_center) );
        auto norm = (i_center + vec3(1, 1, 0)) * 500.0f;
        vec3 dp = dist * vec3(rando::next_range(), rando::next_range(), 0);
        return i_center + dp;
    };

    r.setIterations(2);
    r.setGenerator(circular);
    r.setJitter(center_out);

    render.setSettings(r);
    render.generate_image();

    vector<vec3> s;
    render.m_b->make_samples(s, 300);

// Textures
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    t.setup();
    m.setup();


    return m;
 }


void sample_curve(    const int num_samples = 1200,
                     const int num_discs = 20)
{

    vector<vec3> s;
    for(int i=0; i < num_discs; i++){
        render.m_b->jiggle(render.getSettings().getJitter());
        render.m_b->make_samples(s, num_samples);
    }
    glBindVertexArray(m.spline_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.splineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (s.size()), s.data(), GL_STATIC_DRAW);

    int dead_samples = num_samples - 5;
    /* do renderstuff here? */
    glPointSize(2);
    for(int i=0; i < num_discs; i++) {
        glDrawArrays(GL_LINE_STRIP, num_samples * i, dead_samples);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

    // Setup ImGui binding
    //ImGui_ImplGlfwGL3_Init(window, true);

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    cout<<"Can i even cout here?"<<endl;

    auto tt_prog = two_triangles();

    cout<<"Guess I can;"<<endl;
    static float f = 0.9f;
    static int energy = 1;
    static int dis = 0;
    int iters =0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        iters ++;
        energy = iters * .8f ;
         glUniform1f(t.layout.gamma, f);
         glUniform1f(t.layout.max_value, pow((float)energy, f));
         glUniform4f(t.layout.end_color, clear_color.x, clear_color.y, clear_color.z, clear_color.w);


        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glBindFramebuffer(GL_FRAMEBUFFER, m.fbo);

        glViewport(0,0,500,500);

        glEnable(GL_BLEND);
        m.activate();
        sample_curve();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0,0,display_w, display_h);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        t.activate();
        glDisable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.colorBuffer);

        glBindVertexArray(t.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwTerminate();

    return 0;
}
