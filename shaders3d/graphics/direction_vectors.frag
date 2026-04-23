#version 460 core

out vec4 FragColor;
flat in int type;

void main() {
    vec3 color;

    type == 0 ? color.r = 1 : color.r = 0; // x red
    type == 1 ? color.g = 1 : color.g = 0; // y green
    type == 2 ? color.b = 1 : color.b = 0; // z blue

    FragColor = vec4(color, 1.0f);
}