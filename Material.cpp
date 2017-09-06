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



    layout.gamma     = glGetUniformLocation(m_program, "gamma");
    layout.max_value = glGetUniformLocation(m_program, "energy");
    layout.end_color = glGetUniformLocation(m_program, "end_color");


    // Data for those sweet, sweet two triangles
    VertLayout verts[] = {{ -.9f,  .9f,  0,  1,},
                          { -.9f, -.9f,  0,  0,},
                          {  .9f,  .9f,  1,  1,},
                          {  .9f, -.9f,  1,  0,},
                          };
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

// Linking shader attributes to cpp structs
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, x));

    GLint texAttrib = glGetAttribLocation(m_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertLayout), (void*) offsetof(VertLayout, u) );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


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


    glGenVertexArrays(1, &spline_vao);
    glBindVertexArray(spline_vao);
    glGenBuffers(1, &splineVBO);

    glBindBuffer(GL_ARRAY_BUFFER, splineVBO);

    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                          sizeof(SplineLayout), (void*) offsetof(SplineLayout, pos) );


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint hdrFBO;
    glGenFramebuffers(1, &hdrFBO);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 500, 500, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    fbo = hdrFBO;
    return true;
}
