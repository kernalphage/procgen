#version 330

in vec2 Texcoord;
out vec4 outColor;
uniform isampler2D tex;
uniform float gamma;
uniform float energy;
uniform vec4 end_color;

void main()
{
    // ivec4 val = texelFetch(tex, uv, 0);
    ivec4 val = texture(tex, Texcoord);
    float lum =  pow(val.r, gamma)/energy;

    outColor = lum * end_color;
}
