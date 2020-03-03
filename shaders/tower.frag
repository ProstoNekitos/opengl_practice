#version 330

uniform sampler2D diffuse;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
