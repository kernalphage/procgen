#version 330

in vec3 position;

out vec4 color;

void main() {
    gl_Position = vec4(position, 1.0);

    color = vec4(1, 0, atan(position.y, position.x), .01f);
}
