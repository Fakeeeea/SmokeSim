#version 460 core

out vec4 FragColor;

flat in float velocity_value;

void main() {
    float is_negative = velocity_value < 0 ? 1 : 0;
    float intensity = abs(velocity_value);
    FragColor = vec4(intensity * is_negative, intensity * (1 - is_negative), 0.5f, 1.0f);
}