#version 460 core

out vec4 FragColor;

void main() {
    float depth = gl_FragCoord.z;
    FragColor = vec4(vec3(1.0f), 0.15 - depth * 0.1);
}