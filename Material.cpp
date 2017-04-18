//
// Created by matt on 4/14/17.
//

#include "Material.hpp"

bool TwoTriangles::setup() {

    //Create program
    auto vert = create_shader("shaders/twotris_vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("shaders/twotris_frag.glsl", GL_FRAGMENT_SHADER);

    m_program = glCreateProgram();
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glBindFragDataLocation(m_program, 0, "outColor");

    glLinkProgram(m_program);
    glUseProgram(m_program);

// Linking shader attributes to cpp structs
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, x));

    GLint texAttrib = glGetAttribLocation(m_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, u) );

    layout.gamma     = glGetUniformLocation(m_program, "gamma");
    layout.max_value = glGetUniformLocation(m_program, "energy");
    layout.end_color = glGetUniformLocation(m_program, "end_color");

    return true;
}

bool SplineMat::setup() {
    //Create program
    auto vert = create_shader("shaders/spline_vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("shaders/spline_frag.glsl", GL_FRAGMENT_SHADER);

    m_program = glCreateProgram();
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glBindFragDataLocation(m_program, 0, "outColor");

    glLinkProgram(m_program);
    glUseProgram(m_program);

    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                          sizeof(SplineLayout), (void*) offsetof(SplineLayout, pos) );

    /*
    GLuint hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    GLuint colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 500, 500, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
    return true;
}
