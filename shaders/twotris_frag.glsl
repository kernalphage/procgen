#version 330

in vec2 Texcoord;
out vec4 outColor;
uniform sampler2D tex;
uniform float gamma;
uniform float energy;
uniform vec4 end_color;

void main()
{
    // ivec4 val = texelFetch(tex, uv, 0);
    vec4 val = texture(tex, Texcoord);
    vec4 lum =  pow(val, vec4(gamma))/energy;
    float t = 0;

    outColor = vec4(lum.rg, gamma , 1);
}
