//
// Created by matt on 4/14/17.
//

#ifndef PROCGEN_MATERIAL_HPP
#define PROCGEN_MATERIAL_HPP
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>

#include "helper.hpp"
using namespace std;

class Material {
public:
    virtual bool setup() = 0;
    virtual bool activate(){
        glUseProgram(this->m_program);
    };


protected:

    GLuint m_program;

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
};


class TwoTriangles : public Material{
public:
    bool setup() override ;
    UniformLayout layout;
    GLuint vao;
};

class SplineMat : public Material{
public:
    bool setup() override ;
    GLuint colorBuffer;
    GLuint fbo;

    GLuint spline_vao, splineVBO;
};


#endif //PROCGEN_MATERIAL_HPP
