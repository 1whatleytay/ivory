#version 410

in vec2 fragTexCoord;

uniform sampler2D tex;

out vec4 outputColor;

void main() {
    outputColor = texture(tex, fragTexCoord);
}
