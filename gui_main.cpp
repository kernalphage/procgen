// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include "RenderSettings.hpp"
#include "JitterRenderer.hpp"
#include "rando.hpp"

render_settings r;
const int im_sz = 64;
constexpr int imDim = im_sz * im_sz;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

string read_contents(string filename){
    std::ifstream t(filename);
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
    return str;
}

GLuint create_shader(string filename, GLuint shader_type){
    auto src = read_contents(filename);
    const char* c_src = src.c_str(); // makes shader_source easier

    auto shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &c_src, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status){
        cout<<"When loading "<<filename<<", code: "<<status;
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        cout<<buffer;
    }
    return shader;
}

struct VertLayout{
    float x,y,u,v;
};

struct UniformLayout{
    GLint gamma;
    GLint max_value;
    GLint end_color;
}  g_layout;

auto two_triangles(void){

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Data for those sweet, sweet two triangles
    VertLayout verts[] = {{ -.9f,  .9f,  0,  1,},
                          {  .9f,  .9f,  1,  1,},
                          {  .9f, -.9f,  1,  0,},
                          { -.9f, -.9f,  0,  0,},};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

// Textures
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    /* //float version
    int i=0;
    std::generate(pixels.begin(), pixels.end(), [&]{return di * i++;});
    pixels[0] = 2;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, im_sz,im_sz ,0, GL_RED, GL_FLOAT, pixels.data());
     */

    render_settings r;
    r.width = 500;
    r.height = 500;
    r.num_pts = 100;
    r.iterations = 5000;

    r.need_render = true;
    auto circular = [&] (int cur, int max){
        float di = 3.141 * 2 / (max - 2);
        float theta = di * cur;
        return icomplex(sin(theta), cos(theta)) * .8f;
    };

    auto center_out = [=](icomplex i_center) {
        float dist = 1 / (100 * abs(i_center.real()) + 1);
        auto norm = (i_center + icomplex(1, 1)) * 500.0f;
        icomplex dp = dist * icomplex(rando::next_position(norm, 1), rando::next_position(norm, 100));
        return i_center + dp;
    };

    r.setGenerator(circular);
    r.setJitter(center_out);


    JitterRenderer render;
    render.setSettings(r);
    render.generate_image();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, r.width, r.height, 0,
                 GL_RED_INTEGER, GL_INT, render.accumulator());

    //Create program
    auto vert = create_shader("shaders/vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("shaders/frag.glsl", GL_FRAGMENT_SHADER);

    auto shader_program = glCreateProgram();
    glAttachShader(shader_program, vert);
    glAttachShader(shader_program, frag);
    glBindFragDataLocation(shader_program, 0, "outColor");

    glLinkProgram(shader_program);
    glUseProgram(shader_program);

    // Linking shader attributes to cpp structs
    GLint posAttrib = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, x));

    GLint texAttrib = glGetAttribLocation(shader_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, u) );

    g_layout.gamma = glGetUniformLocation(shader_program, "gamma");
    g_layout.max_value = glGetUniformLocation(shader_program, "energy");
    g_layout.end_color = glGetUniformLocation(shader_program, "end_color");

    return std::make_tuple(shader_program, vao);
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
    GLuint prog, vao;

    std::tie(prog, vao) = two_triangles();
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


        glUniform1f(g_layout.gamma, f);
        glUniform1f(g_layout.max_value, pow((float)energy, f));
        glUniform4f(g_layout.end_color, clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        glClearColor(clear_color.x * .6f, clear_color.y* .6f, clear_color.z* .6f, clear_color.w);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        auto dis = min(display_w, display_h);
        glViewport(0, 0, dis, dis);


        glClear(GL_COLOR_BUFFER_BIT);
        /* do renderstuff here? */
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
