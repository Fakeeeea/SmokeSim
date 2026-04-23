#version 460 core
in vec2 TexCoords;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D text;

uniform vec3 text_color;

void main() {
    FragColor = vec4(text_color, texture(text, TexCoords).r);
}