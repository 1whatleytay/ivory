#version 410

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

uniform vec2 offset;
uniform vec2 scale;

out vec3 fragColor;

void main() {
    gl_Position = vec4(2 * (offset + position) / scale, 0, 1);
    fragColor = color;
}
