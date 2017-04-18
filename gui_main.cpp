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
GLuint spline_vao;
GLuint splineVBO;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

auto two_triangles(void){



    render_settings r;
    r.width = 500;
    r.height = 500;
    r.num_pts = 100;
    r.iterations = 5000;

    r.need_render = true;
    auto circular = [&] (int cur, int max){
        float di = 3.141 * 2 / (max - 2);
        float theta = di * cur;
        return vec3(sin(theta), cos(theta), 0) * .8f;
    };

    auto center_out = [=](vec3 i_center) {
        float dist = .01 * ( std::abs(i_center.x) );
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

    glGenVertexArrays(1, &spline_vao);
    glBindVertexArray(spline_vao);

    glGenBuffers(1, &splineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, splineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (s.size()), s.data(), GL_STREAM_DRAW);

    /*

    // Data for those sweet, sweet two triangles
    VertLayout verts[] = {{ -.9f,  .9f,  0,  1,},
                          {  .9f,  .9f,  1,  1,},
                          {  .9f, -.9f,  1,  0,},
                          { -.9f, -.9f,  0,  0,},};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    */

// Textures
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //TwoTriangles t{};
    //t.setup();


    SplineMat m{};
    m.setup();


    /* //float version
    int i=0;
    std::generate(pixels.begin(), pixels.end(), [&]{return di * i++;});
    pixels[0] = 2;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, im_sz,im_sz ,0, GL_RED, GL_FLOAT, pixels.data());
     */

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    return m;
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
    ImGui_ImplGlfwGL3_Init(window, true);

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    cout<<"Can i even cout here?"<<endl;

    auto tt_prog = two_triangles();

    cout<<"Guess I can;"<<endl;
    static float f = 0.5f;
    static int energy = 5000;
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {

            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("gamma", &f, 0.0f, 1.0f);
            ImGui::SliderInt("energy", &energy, 0, 10000);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }


        bool needs_render = false;
        // spline renderer gui
        {
            ImGui::Begin("Render Settings");
            needs_render |= ImGui::SliderInt("iterations", &(r.iterations), 100, 10000);
            needs_render |= ImGui::SliderFloat("Amplitude", &(r.amplitude), 0,10);
            needs_render |= ImGui::SliderInt("Width/Height", &(r.width), 100, 4096);
            needs_render |= ImGui::SliderInt("Control Points", &(r.num_pts), 5, 200);

            ImGui::End();
        }


        int num_samples = 400;
        const int num_discs = 30;

        vector<vec3> s;
        for(int i=0; i < num_discs; i++){
            render.m_b->jiggle(render.getSettings().getJitter());
            render.m_b->make_samples(s, num_samples);
        }
        glBindVertexArray(spline_vao);
        glBindBuffer(GL_ARRAY_BUFFER, splineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (s.size()), s.data(), GL_STREAM_DRAW);

        // glUniform1f(tt_prog.layout.gamma, f);
        // glUniform1f(tt_prog.layout.max_value, pow((float)energy, f));
        // glUniform4f(tt_prog.layout.end_color, clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        //glClearColor(clear_color.x * .6f, clear_color.y* .6f, clear_color.z* .6f, .01f);
        //glClear(GL_COLOR_BUFFER_BIT);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        auto dis = std::min(display_w, display_h);
        dis *= .98f;
        glViewport(0, 0, dis, dis);

        int dead_samples = num_samples - 5;
        /* do renderstuff here? */
        glPointSize(2);
        for(int i=0; i < num_discs; i++) {
            glDrawArrays(GL_POINTS, num_samples * i, dead_samples);
        }
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
