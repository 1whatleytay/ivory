#version 410

in vec2 fragTexCoord;

uniform sampler2D tex;

out vec4 outputColor;

void main() {
    outputColor = texture(tex, fragTexCoord);

    if (outputColor.a < 0.1)
        discard;

    outputColor.xyz = vec3(0, 0, 0);
}
